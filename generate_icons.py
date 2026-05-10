#!/usr/bin/env python3
"""Generate color icons for Rails watchface (Emery 260x260 display)."""

from PIL import Image, ImageDraw, ImageFont
import os

OUTPUT_DIR = "resources/images"
os.makedirs(OUTPUT_DIR, exist_ok=True)

def create_app_icon():
    """Create a 260x260 app icon with 'R' letter."""
    img = Image.new('RGB', (260, 260), (0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw rounded rectangle background
    draw.rounded_rectangle([10, 10, 250, 250], radius=40, fill=(30, 30, 30))
    
    # Draw 'R' letter
    try:
        font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 150)
    except:
        font = ImageFont.load_default()
    
    bbox = draw.textbbox((0, 0), "R", font=font)
    text_w = bbox[2] - bbox[0]
    text_h = bbox[3] - bbox[1]
    draw.text(((260 - text_w) // 2, (260 - text_h) // 2 - bbox[1]), "R", fill=(255, 255, 255), font=font)
    
    img.save(os.path.join(OUTPUT_DIR, "app_icon.png"), "PNG")
    print("Created app_icon.png")

def create_battery_icons():
    """Create battery state icons."""
    for name, color in [("battery_full.png", (0, 200, 0)), 
                         ("battery_mid.png", (200, 200, 0)),
                         ("battery_low.png", (200, 50, 0)),
                         ("battery_empty.png", (100, 100, 100))]:
        img = Image.new('RGB', (32, 32), (0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Battery outline
        draw.rectangle([4, 4, 24, 24], outline=color, width=2)
        draw.rectangle([26, 10, 28, 20], fill=color)
        
        # Battery fill
        fill_color = color
        draw.rectangle([6, 6, 22, 22], fill=fill_color)
        
        img.save(os.path.join(OUTPUT_DIR, name), "PNG")
        print(f"Created {name}")

def create_status_icons():
    """Create phone and notification icons."""
    # Phone connected icon
    img = Image.new('RGB', (32, 32), (0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle([8, 4, 24, 28], radius=4, outline=(255, 255, 255), width=2)
    for i in range(3):
        draw.rectangle([12 + i*3, 10 + i*3, 13 + i*3, 11 + i*3], fill=(255, 255, 255))
    img.save(os.path.join(OUTPUT_DIR, "phone_connected.png"), "PNG")
    print("Created phone_connected.png")
    
    # Notification icon
    img = Image.new('RGB', (32, 32), (0, 0, 0))
    draw = ImageDraw.Draw(img)
    draw.rounded_rectangle([10, 8, 22, 18], radius=2, fill=(255, 50, 50))
    draw.rectangle([14, 4, 18, 8], fill=(255, 50, 50))
    img.save(os.path.join(OUTPUT_DIR, "notification.png"), "PNG")
    print("Created notification.png")

def create_health_icons():
    """Create health tracking icons."""
    icons = {
        "heart.png": (255, 50, 50),
        "step_icon.png": (50, 200, 50),
        "calorie_icon.png": (255, 150, 0),
        "distance_icon.png": (50, 200, 255),
        "floor_icon.png": (255, 255, 50),
    }
    
    for name, color in icons.items():
        img = Image.new('RGB', (32, 32), (0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        if name == "heart.png":
            # Heart shape
            draw.rectangle([8, 8, 16, 16], fill=color)
            draw.rectangle([16, 8, 24, 16], fill=color)
            draw.rectangle([10, 16, 22, 24], fill=color)
        elif name == "step_icon.png":
            # Foot shape
            draw.rectangle([10, 20, 16, 28], fill=color)  # heel
            draw.rectangle([12, 12, 18, 20], fill=color)  # foot
            draw.rectangle([14, 6, 18, 12], fill=color)   # toe
        elif name == "calorie_icon.png":
            # Flame shape
            draw.rectangle([10, 14, 22, 22], fill=color)
            draw.rectangle([12, 6, 20, 14], fill=color)
            draw.rectangle([14, 2, 18, 6], fill=color)
        elif name == "distance_icon.png":
            # Pin shape
            draw.rectangle([12, 4, 20, 16], fill=color)
            draw.rectangle([14, 16, 18, 26], fill=color)
        elif name == "floor_icon.png":
            # Stairs
            for i in range(3):
                draw.rectangle([4 + i*8, 20 - i*8, 12 + i*8, 28 - i*8], fill=color)
        
        img.save(os.path.join(OUTPUT_DIR, name), "PNG")
        print(f"Created {name}")

def create_weather_icons():
    """Create weather condition icons."""
    icons = {
        "weather_sun.png": (255, 255, 0),
        "weather_cloud.png": (200, 200, 200),
        "weather_rain.png": (100, 150, 255),
    }
    
    for name, color in icons.items():
        img = Image.new('RGB', (32, 32), (0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        if name == "weather_sun.png":
            draw.rectangle([10, 10, 22, 22], fill=color)
            for angle in [0, 45, 90, 135, 180, 225, 270, 315]:
                rad = angle * 3.14159 / 180
                cx, cy = 16, 16
                sx, sy = int(cx + 8 * cos(rad)), int(cy + 8 * sin(rad))
                ex, ey = int(cx + 12 * cos(rad)), int(cy + 12 * sin(rad))
                draw.line([(sx, sy), (ex, ey)], fill=color, width=2)
        elif name == "weather_cloud.png":
            draw.rectangle([8, 14, 24, 22], fill=color)
            draw.rectangle([12, 10, 20, 14], fill=color)
        elif name == "weather_rain.png":
            draw.rectangle([8, 10, 24, 18], fill=(200, 200, 200))
            for i in range(3):
                draw.rectangle([10 + i*6, 20, 12 + i*6, 26], fill=color)
        
        img.save(os.path.join(OUTPUT_DIR, name), "PNG")
        print(f"Created {name}")

def create_settings_icon():
    """Create settings gear icon."""
    img = Image.new('RGB', (32, 32), (0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Simple gear shape
    draw.circle([16, 16], 10, outline=(255, 255, 255), width=2)
    draw.circle([16, 16], 5, fill=(0, 0, 0))
    
    img.save(os.path.join(OUTPUT_DIR, "settings_icon.png"), "PNG")
    print("Created settings_icon.png")

if __name__ == "__main__":
    import math
    cos = math.cos
    sin = math.sin
    
    create_app_icon()
    create_battery_icons()
    create_status_icons()
    create_health_icons()
    create_weather_icons()
    create_settings_icon()
    print("\nAll icons generated!")
