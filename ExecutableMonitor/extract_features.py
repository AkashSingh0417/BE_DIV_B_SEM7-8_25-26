import os
import hashlib
import math
import mimetypes
import re
import binascii
import struct
import string
from datetime import datetime
from collections import Counter

# Try to import magic, but provide a fallback if it's not available
try:
    import magic
    MAGIC_AVAILABLE = True
except ImportError:
    print("Warning: python-magic or libmagic is not installed. Some file type detection features will be limited.")
    MAGIC_AVAILABLE = False

def get_file_entropy(file_path):
    with open(file_path, "rb") as f:
        data = f.read()
    if not data:
        return 0
    byte_freq = [0] * 256
    for b in data:
        byte_freq[b] += 1
    entropy = 0
    for freq in byte_freq:
        if freq > 0:
            p = freq / len(data)
            entropy -= p * math.log2(p)
    return round(entropy, 4)

def get_sha256(file_path):
    sha256 = hashlib.sha256()
    with open(file_path, "rb") as f:
        while chunk := f.read(8192):
            sha256.update(chunk)
    return sha256.hexdigest()

def is_executable(file_path):
    return file_path.lower().endswith(('.exe', '.dll', '.bat', '.cmd', '.ps1', '.vbs', '.js', '.msi', '.scr'))

def get_file_extension(file_path):
    return os.path.splitext(file_path)[1].lower()

def get_mime_type(file_path):
    mime_type, _ = mimetypes.guess_type(file_path)
    return mime_type or "unknown"

def get_file_magic(file_path):
    if not MAGIC_AVAILABLE:
        return "magic module not available"
    try:
        return magic.from_file(file_path)
    except Exception:
        return "unknown"

def get_file_header(file_path, num_bytes=20):
    try:
        with open(file_path, 'rb') as f:
            header = f.read(num_bytes)
            return binascii.hexlify(header).decode('utf-8')
    except Exception:
        return ""

def get_strings(file_path, min_length=4, max_strings=100):
    try:
        with open(file_path, 'rb') as f:
            content = f.read()
            # Find ASCII strings
            ascii_pattern = re.compile(b'[\x20-\x7E]{' + str(min_length).encode() + b',}')
            ascii_strings = ascii_pattern.findall(content)
            # Find UTF-16 strings (Windows)
            utf16_pattern = re.compile(b'(?:([\x20-\x7E]\x00){' + str(min_length).encode() + b',})')
            utf16_strings = [s.decode('utf-16le', errors='ignore') for s in utf16_pattern.findall(content)]
            # Combine and limit
            all_strings = [s.decode('ascii', errors='ignore') for s in ascii_strings] + utf16_strings
            return all_strings[:max_strings]
    except Exception:
        return []

def analyze_pe_file(file_path):
    try:
        if not file_path.lower().endswith('.exe') and not file_path.lower().endswith('.dll'):
            return {}
            
        with open(file_path, 'rb') as f:
            # Check MZ header
            if f.read(2) != b'MZ':
                return {}
                
            # Get PE header offset
            f.seek(0x3C)
            pe_offset = struct.unpack('<I', f.read(4))[0]
            
            # Check PE signature
            f.seek(pe_offset)
            if f.read(4) != b'PE\x00\x00':
                return {}
                
            # Get number of sections
            f.seek(pe_offset + 6)
            num_sections = struct.unpack('<H', f.read(2))[0]
            
            # Get timestamp
            f.seek(pe_offset + 8)
            timestamp = struct.unpack('<I', f.read(4))[0]
            
            # Get characteristics
            f.seek(pe_offset + 22)
            characteristics = struct.unpack('<H', f.read(2))[0]
            
            return {
                "pe_sections": num_sections,
                "pe_timestamp": timestamp,
                "pe_characteristics": characteristics,
                "is_dll": bool(characteristics & 0x2000),
                "is_system": bool(characteristics & 0x1000),
                "is_gui": bool(characteristics & 0x2),
            }
    except Exception:
        return {}

def check_digital_signature(file_path):
    # This is a simplified check - in a real implementation, you would use libraries like pywin32
    # to verify digital signatures properly
    try:
        if os.name == 'nt' and file_path.lower().endswith(('.exe', '.dll', '.sys')):
            # Check for certificate data in the file (simplified approach)
            with open(file_path, 'rb') as f:
                content = f.read()
                # Look for common certificate markers
                has_cert_markers = (b'Microsoft Corporation' in content or 
                                  b'DigiCert' in content or 
                                  b'VeriSign' in content or
                                  b'GlobalSign' in content)
                return has_cert_markers
        return False
    except Exception:
        return False

def extract_file_features(file_path):  # ✅ Make sure this is defined!
    try:
        stat = os.stat(file_path)
        
        # Basic features
        features = {
            "file_path": file_path,
            "file_size": stat.st_size,
            "extension": get_file_extension(file_path),
            "mime_type": get_mime_type(file_path),
            "magic_type": get_file_magic(file_path),
            "sha256": get_sha256(file_path),
            "entropy": get_file_entropy(file_path),
            "is_executable": is_executable(file_path),
            "created_at": datetime.fromtimestamp(stat.st_ctime).isoformat(),
            "modified_at": datetime.fromtimestamp(stat.st_mtime).isoformat(),
            "file_header": get_file_header(file_path),
            "has_digital_signature": check_digital_signature(file_path),
            "strings_count": 0,
            "suspicious_count": 0
        }
        # Add PE file analysis for executables
        if is_executable(file_path):
            pe_features = analyze_pe_file(file_path)
            features.update(pe_features)
            
        # Extract strings from the file
        strings_found = get_strings(file_path)
        features["strings_sample"] = strings_found[:10]  # Just include a sample of strings
        features["strings_count"] = len(strings_found)
        
        # Check for potentially suspicious strings
        suspicious_keywords = [
            "cmd.exe", "powershell", "http://", "https://", 
            "system32", "regedit", "taskkill", "netstat", 
            "password", "admin", "administrator", "root", 
            "exec", "eval", "execute", "shell", "spawn", 
            "download", "upload", "inject", "payload",
            "malware", "virus", "trojan", "backdoor",
            "keylogger", "ransomware", "botnet", "cryptocurrency"
        ]
        
        suspicious_found = []
        for keyword in suspicious_keywords:
            for string in strings_found:
                if keyword.lower() in string.lower():
                    suspicious_found.append(f"{keyword}: {string}")
                    break
        
        features["suspicious_strings"] = suspicious_found
        features["suspicious_count"] = len(suspicious_found)
        
        return features
    except Exception as e:
        print(f"[ERROR] Failed to extract features: {e}")
        return None
