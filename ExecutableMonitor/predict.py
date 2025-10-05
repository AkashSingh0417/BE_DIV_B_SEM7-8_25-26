import os
import re

# List of suspicious file extensions
SUSPICIOUS_EXTENSIONS = [
    '.exe', '.dll', '.bat', '.cmd', '.ps1', '.vbs', '.js', '.jar', '.msi', '.scr',
    '.pif', '.hta', '.cpl', '.com', '.reg', '.gadget', '.msc', '.msp', '.mst', '.inf'
]

# List of suspicious patterns in file content
SUSPICIOUS_PATTERNS = [
    rb'powershell', rb'cmd.exe', rb'rundll32', rb'wscript', rb'cscript',
    rb'regsvr32', rb'bitsadmin', rb'certutil', rb'mshta', rb'regasm',
    rb'installutil', rb'regsvcs', rb'msbuild', rb'dnscmd', rb'netsh',
    rb'psexec', rb'wmic', rb'mimikatz', rb'PsExec', rb'procdump',
    rb'<script>', rb'eval\(', rb'document\.write', rb'fromCharCode',
    rb'CreateObject', rb'WScript.Shell', rb'ActiveXObject', rb'ShellExecute',
    rb'wget', rb'curl', rb'Invoke-WebRequest', rb'DownloadFile',
    rb'System.Net.WebClient', rb'Start-Process', rb'CreateProcess',
    rb'exec\(', rb'spawn\(', rb'child_process', rb'shell_exec',
    rb'system\(', rb'passthru', rb'proc_open', rb'popen'
]

def predict_file(file_path):
    """
    Perform basic rule-based safety prediction on a file.
    Returns 'suspicious' or 'safe'
    """
    # Check if file exists
    if not os.path.exists(file_path):
        return "suspicious"  # Treat missing files as suspicious
    
    # Check file extension
    _, ext = os.path.splitext(file_path.lower())
    if ext in SUSPICIOUS_EXTENSIONS:
        return "suspicious"
    
    # Check file size
    try:
        file_size = os.path.getsize(file_path)
        if file_size > 10 * 1024 * 1024:  # Files larger than 10MB
            return "suspicious"
        
        # For smaller files, check content for suspicious patterns
        if file_size < 5 * 1024 * 1024:  # Only scan files smaller than 5MB
            try:
                with open(file_path, 'rb') as f:
                    content = f.read()
                    for pattern in SUSPICIOUS_PATTERNS:
                        if re.search(pattern, content, re.IGNORECASE):
                            return "suspicious"
            except Exception:
                # If we can't read the file, consider it suspicious
                return "suspicious"
    except Exception:
        return "suspicious"
    
    # If no suspicious indicators found
    return "safe"