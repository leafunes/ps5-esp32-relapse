#!/usr/bin/env python3
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT.parent / "Relapse-Exploit"
DATA = ROOT / "data"

MENU_LINK = '<a href="/" style="position:fixed;top:10px;left:10px;'
MENU_LINK += 'background:#222;color:#0ff;padding:6px 10px;text-decoration:none;'
MENU_LINK += 'border-radius:4px;z-index:9999;">\u2190 Menu</a>'

for name in ["src", "offsets", "payloads"]:
    src = SRC / name
    dst = DATA / name
    if not src.exists():
        print(f"skip (no existe): {src}")
        continue
    if dst.exists():
        shutil.rmtree(dst)
    shutil.copytree(src, dst)

exploit = SRC / "index.html"
if exploit.exists():
    html = exploit.read_text()
    if "Menu</a>" not in html:
        html = html.replace("<body>", "<body>\n  " + MENU_LINK + "\n", 1)
    (DATA / "exploit.html").write_text(html)
    print("exploit.html generado (con enlace al menu)")

print("Assets copiados a", DATA)
print("(index.html del menu y pl/ no se tocan)")
