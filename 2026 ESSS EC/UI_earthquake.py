import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
import serial

# CONFIGURATION
PORT = "COM3"
BAUD = 115200
MAX_POINTS = 200

time_data, accel_data, vel_data, dist_data = [], [], [], []
current_v, current_d, last_time_s = 0.0, 0.0, None
peak_g = 0.0  # variable for G force

# Styling
plt.style.use('dark_background')
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

# Background colors
BG_COLOR = '#1e1e2e'
PANEL_COLOR = '#282a36'
GRID_COLOR = '#44475a'

fig.patch.set_facecolor(BG_COLOR)
fig.suptitle(
    "SEISMIC TELEMETRY DASHBOARD", 
    fontsize=16, 
    fontweight="bold", 
    color="#f8f8f2", 
    fontfamily="monospace",
    y=0.96
)

# Open Serial connection
ser = serial.Serial(PORT, BAUD, timeout=0.01)

def update(frame):
    global current_v, current_d, last_time_s, peak_g

    while ser.in_waiting > 0:
        try:
            line = ser.readline().decode("utf-8").strip()
            if not line or line.startswith("#"):
                continue

            parts = line.split(",")
            if len(parts) >= 4:
                t_ms = float(parts[0])
                ax = float(parts[1])
                ay = float(parts[2])
                az = float(parts[3])

                accel_mag = np.sqrt(ax * ax + ay * ay + az * az)
                
                # --- PEAK G-FORCE TRACKING ---
                current_g = accel_mag / 9.81
                if current_g > peak_g:
                    peak_g = current_g

                t_s = t_ms / 1000.0
                
                if last_time_s is None:
                    dt = 0.01
                else:
                    dt = t_s - last_time_s
                    if dt <= 0:
                        dt = 0.01
                last_time_s = t_s

                # --- DRIFT CORRECTION & DAMPING ---
                # 1. Noise Deadband (ignore tiny sensor jitter below 0.06 m/s^2)
                net_a = ax
                if abs(net_a) < 0.06:
                    net_a = 0.0

                # 2. Leaky Integration (damps velocity back to 0 when resting)
                current_v = (current_v * 0.94) + (net_a * dt)
                current_d = (current_d * 0.94) + (current_v * dt)

                # Append to buffers
                time_data.append(t_s)
                accel_data.append(accel_mag)
                vel_data.append(current_v)
                dist_data.append(current_d * 100) # Convert to cm

                # Enforce rolling window
                if len(time_data) > MAX_POINTS:
                    time_data.pop(0)
                    accel_data.pop(0)
                    vel_data.pop(0)
                    dist_data.pop(0)
        except Exception:
            pass

    if len(time_data) > 0:
        axes = [ax1, ax2, ax3]
        for ax in axes:
            ax.clear()
            ax.set_facecolor(PANEL_COLOR)
            ax.grid(True, color=GRID_COLOR, linestyle='--', linewidth=0.5, alpha=0.8)
            ax.tick_params(colors='#6272a4', labelsize=10)
            for spine in ax.spines.values():
                spine.set_color(GRID_COLOR)

        # Plot 1: Acceleration (Neon Pink/Red)
        color_accel = "#ff5555"
        ax1.plot(time_data, accel_data, color=color_accel, linewidth=2)
        ax1.fill_between(time_data, accel_data, 0, color=color_accel, alpha=0.15)
        ax1.set_ylabel("Accel (m/s²)", color=color_accel, fontweight='bold')
        ax1.set_ylim(-0.1, max(4.0, max(accel_data) * 1.2))
        
        # Peak G-Force Readout
        ax1.text(0.98, 0.85, f"PEAK: {peak_g:.3f} G", transform=ax1.transAxes, color="#f1fa8c", fontsize=12, fontweight='bold', ha='right')

        # Plot 2: Velocity (Neon Amber)
        color_vel = "#ffb86c"
        ax2.plot(time_data, vel_data, color=color_vel, linewidth=2)
        ax2.fill_between(time_data, vel_data, 0, color=color_vel, alpha=0.15)
        ax2.set_ylabel("Velocity (m/s)", color=color_vel, fontweight='bold')
        ax2.set_ylim(-0.5, 0.5)

        # Plot 3: Displacement (Neon Cyan)
        color_dist = "#8be9fd"
        ax3.plot(time_data, dist_data, color=color_dist, linewidth=2)
        ax3.fill_between(time_data, dist_data, 0, color=color_dist, alpha=0.15)
        ax3.set_ylabel("Disp (cm)", color=color_dist, fontweight='bold')
        ax3.set_xlabel("Time (seconds)", color="#f8f8f2", fontweight='bold')
        ax3.set_ylim(-5.0, 5.0)

ani = animation.FuncAnimation(fig, update, interval=33)
plt.tight_layout()

# Adjust spacing so the title doesn't overlap the top plot
plt.subplots_adjust(top=0.92, hspace=0.15) 
plt.show()

# Clean close on exit
ser.close()
