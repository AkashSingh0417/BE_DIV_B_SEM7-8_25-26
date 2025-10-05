import os
import json
import time
import threading
from fastapi import FastAPI
from fastapi.responses import FileResponse, JSONResponse, Response
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import google.generativeai as genai
from dotenv import load_dotenv

# Import from local files
from extract_features import extract_file_features
from predict import predict_file

# Load environment variables
load_dotenv()

# Initialize FastAPI app
app = FastAPI(title="File Watcher API")

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Global variables
WATCHED_DIR = "C:/Users/hp/Downloads"  # Directory to monitor
analyzed_files = []  # Store analyzed files
observer = None  # Watchdog observer instance

class FileEventHandler(FileSystemEventHandler):
    def __init__(self):
        self.api_key = os.getenv("GEMINI_API_KEY")
        if self.api_key:
            genai.configure(api_key=self.api_key)
        
    def on_created(self, event):
        if not event.is_directory:
            file_path = event.src_path
            print(f"[INFO] New file detected: {file_path}")
            
            # Add file to list with 'analyzing' status
            file_name = os.path.basename(file_path)
            file_info = {
                'name': file_name,
                'path': file_path,
                'type': 'analyzing',
                'details': None
            }
            
            # Add to analyzed files list - clients will poll for updates
            analyzed_files.append(file_info)
            
            # Start analysis in a separate thread to not block
            threading.Thread(target=self.analyze_file, args=(file_path, file_info)).start()
    
    def analyze_file(self, file_path, file_info):
        # Add a short delay to wait for the file to finish writing
        time.sleep(1.5)
        
        try:
            # Extract features
            features = extract_file_features(file_path)
            if not features:
                file_info['type'] = 'error'
                return
                
            # Basic safety check
            basic_safety = predict_file(file_path)
            
            # Set initial risk level based on basic check
            if basic_safety == "suspicious":
                file_info['type'] = 'suspicious'
            else:
                file_info['type'] = 'safe'
            
            # Prepare details for UI
            file_size = features.get('file_size', 0)
            file_info['details'] = {
                'size': f"{file_size / (1024 * 1024):.2f} MB" if file_size > 1024*1024 else f"{file_size / 1024:.2f} KB",
                'ext': features.get('extension', 'unknown'),
                'mime': features.get('mime_type', 'unknown'),
                'magic_type': features.get('magic_type', 'unknown'),
                'hash': features.get('sha256', 'unknown'),
                'entropy': str(features.get('entropy', 'unknown')),
                'is_executable': features.get('is_executable', False),
                'has_digital_signature': features.get('has_digital_signature', False),
                'suspicious_strings': features.get('suspicious_strings', []),
                'file_header': features.get('file_header', ''),
                'created_at': features.get('created_at', ''),
                'modified_at': features.get('modified_at', ''),
                'strings_count': features.get('strings_count', 0),
                'suspicious_count': features.get('suspicious_count', 0),
                'pe_sections': features.get('pe_sections', ''),
                'pe_timestamp': features.get('pe_timestamp', ''),
                'is_dll': features.get('is_dll', False),
                'rule': f"File is a {features.get('mime_type', 'unknown')} file. {basic_safety.capitalize()} based on initial checks.",
                'gemini': "Gemini AI analysis not available."
            }
            
            # Add more detailed rule-based assessment
            rule_details = []
            
            # Check for high entropy
            if features.get('entropy', 0) > 7.0:
                rule_details.append("High entropy detected (>7.0), which may indicate encryption, compression, or obfuscation.")
            
            # Check for executable with no digital signature
            if features.get('is_executable', False) and not features.get('has_digital_signature', False):
                rule_details.append("Executable file without a valid digital signature.")
            
            # Check for suspicious strings
            if features.get('suspicious_strings', []):
                rule_details.append(f"Found {len(features.get('suspicious_strings', []))} potentially suspicious strings.")
            
            # Check for extension/mime type mismatch
            if features.get('mime_type', '') != 'unknown' and features.get('magic_type', '') != 'unknown':
                if features.get('extension', '').lower() not in features.get('mime_type', '').lower() and \
                   features.get('extension', '').lower() not in features.get('magic_type', '').lower():
                    rule_details.append("Possible file extension mismatch with actual content type.")
            
            # Update rule text if we have additional details
            if rule_details:
                file_info['details']['rule'] += " " + " ".join(rule_details)
            
            # Advanced analysis with Gemini if API key is available
            if self.api_key:
                gemini_analysis = self.analyze_with_gemini(features)
                if gemini_analysis:
                    file_info['details']['gemini'] = gemini_analysis
                    # Update risk level if Gemini found it suspicious
                    if "suspicious" in gemini_analysis.lower() or "malicious" in gemini_analysis.lower() or "high risk" in gemini_analysis.lower():
                        file_info['type'] = 'suspicious'
            
            # Update the file info in the global list
            # No broadcasting needed - clients will poll for updates
            
        except Exception as e:
            print(f"[ERROR] Analysis failed: {e}")
            file_info['type'] = 'error'
    
    def analyze_with_gemini(self, features):
        try:
            # Convert features to a formatted string for Gemini
            features_str = json.dumps(features, indent=2)
            
            # Create the prompt for Gemini
            prompt = f"""Analyze this file based on its extracted features and determine if it's safe or suspicious:
            
            {features_str}
            
            Focus especially on these security-relevant features:
            - File entropy (high entropy >7.0 may indicate encryption or packing)
            - Executable status and PE file characteristics
            - Digital signature verification
            - Suspicious strings found in the file
            - File header analysis
            - File magic type vs extension mismatch
            
            Please provide:
            1. A safety assessment (Safe, Suspicious, or Malicious)
            2. Detailed explanation of your assessment with evidence
            3. Highlight any suspicious or important characteristics
            4. Risk level (Low, Medium, High)
            5. Specific recommendations for handling this file
            """
            
            # Configure Gemini model with fallback options to avoid 404s on unsupported versions
            supported_models = [
                'gemini-2.0-flash'
            ]

            last_error = None
            for model_name in supported_models:
                try:
                    model = genai.GenerativeModel(model_name)
                    response = model.generate_content(prompt)
                    if hasattr(response, 'text') and response.text:
                        return response.text
                    # Some SDK versions return a list of candidates
                    if hasattr(response, 'candidates') and response.candidates:
                        candidate_text = getattr(response.candidates[0], 'content', None)
                        if candidate_text:
                            return str(candidate_text)
                except Exception as inner_e:
                    last_error = inner_e
                    continue

            # If all models failed
            if last_error:
                raise last_error
            return None
            return response.text
            
        except Exception as e:
            print(f"[ERROR] Gemini analysis failed: {e}")
            return None

# Routes
@app.get("/")
async def get_html():
    # Serve the UI directly as a static file (avoids encoding issues)
    return FileResponse("ui.html", media_type="text/html; charset=utf-8")

@app.get("/favicon.ico")
async def favicon():
    # No favicon provided; return empty response to avoid 404 noise
    return Response(status_code=204)

@app.get("/api/files")
async def get_files():
    return JSONResponse(content=analyzed_files)

@app.get("/api/status")
async def get_status():
    api_key = os.getenv("GEMINI_API_KEY")
    return JSONResponse(content={
        'monitoring': True,
        'watched_dir': WATCHED_DIR,
        'gemini_enabled': bool(api_key) and api_key != 'your_api_key_here',
        'file_count': len(analyzed_files)
    })

def start_monitoring():
    global observer
    if not os.path.isdir(WATCHED_DIR):
        try:
            os.makedirs(WATCHED_DIR, exist_ok=True)
        except Exception as e:
            print(f"[WARN] Could not ensure watched dir exists: {e}")

    if observer is None:
        obs = Observer()
        event_handler = FileEventHandler()
        obs.schedule(event_handler, WATCHED_DIR, recursive=True)
        obs.start()
        observer = obs
        print(f"Monitoring started on: {WATCHED_DIR}")

def stop_monitoring():
    global observer
    if observer is not None:
        observer.stop()
        observer.join()
        observer = None
        print("Monitoring stopped.")

@app.on_event("startup")
def on_startup():
    start_monitoring()

@app.on_event("shutdown")
def on_shutdown():
    stop_monitoring()

# Main entry point
def main():
    # FastAPI will handle watcher lifecycle via startup/shutdown events
    uvicorn.run(app, host="0.0.0.0", port=5000)

if __name__ == '__main__':
    main()