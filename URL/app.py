from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import uvicorn
import pickle
import numpy as np
from feature_extr import FeatureExtraction
import os, sys, importlib


app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


# Load phishing detection model (GradientBoostingClassifier)
phishing_model = pickle.load(open("gbc_final_model.pkl", "rb"))


class UrlPayload(BaseModel):
    url: str


@app.post("/analyze_url")
def analyze_url(payload: UrlPayload):
    url = payload.url

    # Extract up to 30 features for the URL
    feature_extractor = FeatureExtraction(url)
    features = feature_extractor.getFeaturesList()
    max_features = 30
    features = features[:max_features] + [None] * (max_features - len(features))

    FEATURE_INFO = [
        ("Using IP (UsingIP)", "If the domain contains an IP address instead of a domain name, it's more likely to be phishing."),
        ("Long URL (LongURL)", "Long URLs are often used to hide malicious parameters."),
        ("Short URL (ShortURL)", "Shortened URLs can obscure the real destination and may lead to phishing websites."),
        ("Symbol '@' (Symbol@)", "The '@' symbol in a URL is often used in phishing attacks to create fake subdomains."),
        ("Redirecting with // (Redirecting//)", "URLs with multiple forward slashes can be used for redirection and deception."),
        ("Prefix-Suffix in Domain (PrefixSuffix-)", "A hyphen in the domain name is often a sign of phishing attempts."),
        ("Subdomains (SubDomains)", "Excessive subdomains can be used to mimic legitimate sites."),
        ("HTTPS (HTTPS)", "The presence of HTTPS does not guarantee safety but increases legitimacy."),
        ("Domain Registration Length (DomainRegLen)", "Short registration periods indicate a higher likelihood of phishing."),
        ("Favicon (Favicon)", "If the favicon is missing or mismatched, it might indicate phishing."),
        ("Non-Standard Port (NonStdPort)", "Phishing sites often use uncommon ports to evade detection."),
        ("HTTPS in Domain (HTTPSDomainURL)", "Having 'https' in the domain name instead of using it properly in the URL is suspicious."),
        ("Request URL (RequestURL)", "Phishing sites often load resources from external sources."),
        ("Anchor URL (AnchorURL)", "Links within the page that redirect to suspicious domains indicate phishing."),
        ("Links in Script Tags (LinksInScriptTags)", "If many external links are found in JavaScript, it could indicate phishing."),
        ("Server Form Handler (ServerFormHandler)", "If the form action points to an external domain, it is risky."),
        ("Info Email (InfoEmail)", "Email addresses in page content can indicate phishing."),
        ("Abnormal URL (AbnormalURL)", "If the URL structure deviates from standard formats, it can be suspicious."),
        ("Website Forwarding (WebsiteForwarding)", "Frequent redirections are a known phishing tactic."),
        ("Status Bar Customization (StatusBarCust)", "Altering the browser status bar is a sign of deception."),
        ("Right Click Disable (DisableRightClick)", "Disabling right-click prevents users from investigating the site."),
        ("Popup Window (UsingPopupWindow)", "Excessive pop-ups are often a phishing tactic."),
        ("Iframe Redirection (IframeRedirection)", "Hidden iframes can be used to steal information."),
        ("Age of Domain (AgeofDomain)", "Newly registered domains are more likely to be malicious."),
        ("DNS Record (DNSRecording)", "A missing DNS record suggests that a site might not be trustworthy."),
        ("Website Traffic (WebsiteTraffic)", "Low traffic websites are often malicious."),
        ("PageRank (PageRank)", "A low PageRank means the site is not well-trusted."),
        ("Google Index (GoogleIndex)", "If a site is not indexed by Google, it could be a phishing site."),
        ("Links Pointing to Page (LinksPointingToPage)", "Legitimate sites have more backlinks."),
        ("Statistical Report (StatsReport)", "Phishing sites often appear in blacklists."),
    ]

    feature_descriptions = []
    for i in range(max_features):
        feature_name, full_description = FEATURE_INFO[i]
        feature_value = features[i]
        if feature_value == 1:
            meaning = "✅ Indicates the behaviour of a legitimate website."
        elif feature_value == -1:
            meaning = "⚠️ Indicates phishing behavior."
        elif feature_value == 0:
            meaning = "ℹ️ No strong indication of phishing or legitimacy."
        else:
            meaning = "ℹ️ No data."
        feature_descriptions.append({
            "feature": feature_name,
            "description": meaning,
            "full_description": full_description,
        })

    url_features = np.array([f if f is not None else 0 for f in features]).reshape(1, -1)
    prediction = phishing_model.predict(url_features)[0]
    if prediction == 1:
        classification = "Legitimate"
        conclusion = "✅ Safe to Visit: The analysis indicates that the URL does not exhibit characteristics of phishing. While no automated system is 100% accurate, this website appears to be safe for browsing. However, always exercise caution when entering sensitive information online."
    else:
        classification = "Phishing"
        conclusion = "⚠️ Caution: The URL you entered has been identified as a phishing website. Phishing websites are designed to steal sensitive information such as login credentials, credit card details, or personal data. It is strongly recommended that you do not enter any personal information on this site and avoid interacting with it."

    return {
        "classification": classification,
        "features_table": feature_descriptions,
        "conclusion": conclusion,
    }

# -------------------------
# Executable Monitor wiring
# -------------------------

# Ensure ExecutableMonitor module is importable
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EM_DIR = os.path.join(BASE_DIR, "ExecutableMonitor")
if EM_DIR not in sys.path:
    sys.path.append(EM_DIR)

try:
    em = importlib.import_module("server")
except Exception as e:
    em = None

@app.get("/api/files")
def api_files():
    if em is None:
        return []
    return em.analyzed_files

@app.get("/api/status")
def api_status():
    if em is None:
        return {"monitoring": False, "watched_dir": None, "gemini_enabled": False, "file_count": 0}
    api_key = os.getenv("GEMINI_API_KEY")
    return {
        "monitoring": True,
        "watched_dir": getattr(em, "WATCHED_DIR", None),
        "gemini_enabled": bool(api_key) and api_key != "your_api_key_here",
        "file_count": len(getattr(em, "analyzed_files", [])),
    }

@app.on_event("startup")
def start_exec_monitor():
    if em is not None:
        try:
            em.start_monitoring()
        except Exception:
            pass

@app.on_event("shutdown")
def stop_exec_monitor():
    if em is not None:
        try:
            em.stop_monitoring()
        except Exception:
            pass


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)


