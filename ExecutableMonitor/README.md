# Enhanced File Watcher with Gemini AI Analysis

This project monitors a downloads directory for new files and analyzes them using both rule-based checks and Gemini 2.0 Flash AI to determine if they are safe or suspicious.

## Features

### File Monitoring
- Real-time monitoring of the downloads folder
- Automatic analysis of newly downloaded files

### Advanced File Analysis
- **Basic Features**: Size, extension, MIME type, creation/modification dates
- **Cryptographic**: SHA256 hash, file entropy analysis
- **Content Analysis**: File header examination, magic byte detection
- **String Extraction**: Extracts and analyzes embedded strings
- **Suspicious Pattern Detection**: Identifies potentially malicious strings
- **PE File Analysis**: For Windows executables (.exe, .dll)
  - Section count and characteristics
  - Timestamp analysis
  - File type verification
- **Digital Signature Verification**: Checks for valid signatures

### Security Assessment
- Basic rule-based safety checks
- Advanced AI-powered analysis using Gemini 2.0 Flash
- Detailed safety assessment and recommendations
- Risk level classification

## Setup

1. Install dependencies:
```
pip install -r requirements.txt
```

2. Get a Gemini API key from [Google AI Studio](https://aistudio.google.com/)

3. Set up your API key:
   - Edit the `.env` file and add your Gemini API key: `GEMINI_API_KEY=your_actual_api_key_here`

## Usage

### Web UI

Run the web interface:

```
python server.py
```

Then open your browser to http://localhost:5000

> **Note:** The web UI provides:
> - Real-time monitoring through HTTP polling
> - Interactive file analysis display
> - Automatic API documentation (available at http://localhost:5000/docs)

The program will:
1. Start monitoring your Downloads folder
2. Detect new files as they are downloaded
3. Extract comprehensive file features
4. Perform basic safety checks
5. If a Gemini API key is provided in the .env file, perform advanced AI analysis
6. Display detailed security assessment in the web interface

The web UI provides:
1. Real-time monitoring of your Downloads folder
2. Visual indicators for file safety status
3. Detailed analysis of each file including:
   - File features (size, type, hash, entropy)
   - Rule-based assessment
   - Gemini AI analysis with risk level
4. Interactive file selection and review

## Project Structure

- `server.py`: FastAPI web server with HTTP polling for real-time monitoring
- `ui.html`: Web interface for file analysis visualization
- `extract_features.py`: Advanced file feature extraction utilities
- `predict.py`: Rule-based file safety prediction
- `.env`: Configuration file for storing your Gemini API key

## Customization

To change the monitored directory, modify the `WATCHED_DIR` variable in `server.py`.