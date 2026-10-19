#!/usr/bin/env python3
"""Refresh the Relapse exploit assets in data/ from the upstream repository.

The assets are committed to this repository (so it builds standalone), but this
script re-fetches them from GitHub when you want to update them to a newer
version. Run it from anywhere; it downloads a zip of the pinned ref, copies
`src/`, `offsets/` and `payloads/` into `data/`, and regenerates `exploit.html`
(with the "Menu" link injected).
"""

import shutil
import tempfile
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data"

REPO = "ntfargo/Relapse-Exploit"
# Pin a commit/tag for reproducibility, or use "main" to track latest.
REF = "main"

MENU_LINK = ('<a href="/" style="position:fixed;top:10px;left:10px;'
             'background:#222;color:#0ff;padding:6px 10px;text-decoration:none;'
             'border-radius:4px;z-index:9999;">\u2190 Menu</a>')


def main():
    url = f"https://github.com/{REPO}/archive/{REF}.zip"
    print(f"Downloading {REPO} @ {REF} ...")

    tmp = Path(tempfile.mkdtemp(prefix="relapse-"))
    try:
        zpath = tmp / "repo.zip"
        req = urllib.request.Request(url, headers={"User-Agent": "update_assets.py"})
        with urllib.request.urlopen(req) as resp, open(zpath, "wb") as f:
            shutil.copyfileobj(resp, f)

        with zipfile.ZipFile(zpath) as z:
            z.extractall(tmp)

        src = next((c for c in tmp.iterdir() if c.is_dir()), None)
        if src is None:
            raise RuntimeError("could not locate the extracted repository")

        for name in ["src", "offsets", "payloads"]:
            s = src / name
            d = DATA / name
            if not s.exists():
                print(f"  skip (missing upstream): {name}/")
                continue
            if d.exists():
                shutil.rmtree(d)
            shutil.copytree(s, d)
            print(f"  copied {name}/")

        index = src / "index.html"
        if index.exists():
            html = index.read_text()
            if "Menu</a>" not in html:
                html = html.replace("<body>", "<body>\n  " + MENU_LINK + "\n", 1)
            (DATA / "exploit.html").write_text(html)
            print("  generated exploit.html")
        else:
            print("  warning: index.html not found upstream")
    finally:
        shutil.rmtree(tmp)

    print("Done. Re-run `pio run -t buildfs` (or Upload Filesystem Image) to flash.")


if __name__ == "__main__":
    main()
