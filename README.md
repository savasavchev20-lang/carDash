# OBD-II Live Dashboard

Live car telemetry dashboard — Arduino MEGA + SIM808 GPRS → Web Dashboard

## Files

- `server/server.py` — Flask server that receives data from Arduino
- `server/requirements.txt` — Python dependencies
- `dashboard/index.html` — Live web dashboard
- `GPRS_functions.ino` — Arduino GPRS code to add to main sketch
- `Procfile` — For Railway/Render deployment

---

## Step 1 — Deploy the server (free)

### Option A: Railway (recommended, easiest)
1. Go to https://railway.app and sign up free
2. Click "New Project" → "Deploy from GitHub"
3. Upload this folder to a GitHub repo first, then connect it
4. Railway auto-detects the Procfile and deploys
5. Copy your app URL e.g. `mycarapp.railway.app`

### Option B: Render
1. Go to https://render.com and sign up free
2. New → Web Service → connect your GitHub repo
3. Build command: `pip install -r server/requirements.txt`
4. Start command: `gunicorn server:app --chdir server`
5. Copy your app URL

### Option C: Run locally (for testing)
```bash
pip install flask gunicorn
python server/server.py
```
Server runs at http://localhost:5000

---

## Step 2 — Configure Arduino

In `GPRS_functions.ino`, set your server URL:
```cpp
#define SERVER_HOST  "mycarapp.railway.app"  // your Railway/Render URL
#define GPRS_APN     "internet"              // Vivacom APN
```

Add the GPRS functions to your `OBD2_Complete.ino` and follow
the integration comments at the bottom of `GPRS_functions.ino`.

---

## Step 3 — View dashboard

Open your server URL in any browser on any device.
The dashboard auto-refreshes every 2 seconds.

---

## Vivacom APN settings
- APN: `internet`
- Username: (blank)
- Password: (blank)
