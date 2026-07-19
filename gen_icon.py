"""生成一个简单的十字准心 .ico 图标文件"""
import struct

width, height = 16, 16
bpp = 32

# 像素数据 (BGRA)，背景透明，白色十字准心
pixels = bytearray()
for y in range(height - 1, -1, -1):
    for x in range(width):
        # 十字准心：中心十字线 + 小圆框
        cx, cy = width // 2, height // 2
        on_cross = (x == cx or y == cy) and abs(x - cx) <= 5 and abs(y - cy) <= 5
        on_circle = ((x - cx) ** 2 + (y - cy) ** 2) in [16, 17, 18, 19, 20]
        if on_cross or on_circle:
            pixels.extend([255, 255, 255, 255])  # 白色不透明
        else:
            pixels.extend([0, 0, 0, 0])  # 完全透明

# AND mask (1bpp)
and_mask = bytearray(height * 4)  # 全0 = 不遮罩

# BMP信息头 (BITMAPINFOHEADER)
bmp_header = struct.pack('<IiiHHIIiiII',
    40,           # biSize
    width,        # biWidth
    height * 2,   # biHeight (icon包含XOR+AND)
    1,            # biPlanes
    bpp,          # biBitCount
    0,            # biCompression
    len(pixels) + len(and_mask),
    0, 0, 0, 0
)

image_data = bmp_header + bytes(pixels) + bytes(and_mask)

# ICO文件头
ico_header = struct.pack('<HHH', 0, 1, 1)  # reserved=0, type=1(icon), count=1
ico_entry = struct.pack('<BBBBHHII',
    width if width < 256 else 0,
    height if height < 256 else 0,
    0, 0,        # colors, reserved
    1,           # planes
    bpp,         # bitcount
    len(image_data),
    22           # offset = 6 + 16 = 22
)

with open('O:/clawwork/crosshair/crosshair.ico', 'wb') as f:
    f.write(ico_header + ico_entry + image_data)

print('crosshair.ico generated')
