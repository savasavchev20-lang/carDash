from flask import Flask, request, jsonify, send_from_directory
import json, time, os

app = Flask(__name__, static_folder='../dashboard')

# Latest data store
data = {
    "rpm": 0,
    "spd": 0,
    "cool": 0,
    "thr": 0,
    "vbat": 0.0,
    "lat": 0.0,
    "lon": 0.0,
    "gps_valid": False,
    "gsm_ready": False,
    "obd_ready": False,
    "crash": False,
    "timestamp": 0
}

@app.route('/update', methods=['GET'])
def update():
    """Arduino sends data here via GPRS GET request"""
    global data
    try:
        data['rpm']       = int(request.args.get('rpm', 0))
        data['spd']       = int(request.args.get('spd', 0))
        data['cool']      = int(request.args.get('cool', 0))
        data['thr']       = int(request.args.get('thr', 0))
        data['vbat']      = float(request.args.get('vbat', 0.0))
        data['lat']       = float(request.args.get('lat', 0.0))
        data['lon']       = float(request.args.get('lon', 0.0))
        data['gps_valid'] = request.args.get('gps', '0') == '1'
        data['gsm_ready'] = request.args.get('gsm', '0') == '1'
        data['obd_ready'] = request.args.get('obd', '0') == '1'
        data['crash']     = request.args.get('crash', '0') == '1'
        data['timestamp'] = int(time.time())
        print(f"[UPDATE] SPD={data['spd']} RPM={data['rpm']} GPS={data['lat']},{data['lon']}")
        return "OK", 200
    except Exception as e:
        print(f"[ERROR] {e}")
        return "ERR", 400

@app.route('/data', methods=['GET'])
def get_data():
    """Dashboard polls this for latest data"""
    return jsonify(data)

@app.route('/')
def index():
    return send_from_directory('../dashboard', 'index.html')

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    print(f"Car Dashboard server starting on port {port}")
    app.run(host='0.0.0.0', port=port)
