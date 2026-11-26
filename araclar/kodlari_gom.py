"""Embed kodlar/*.ino into index.html.

Every <code ... data-src="kodlar/x.ino"> element gets the escaped file content.
Run again after changing a sketch:  python3 araclar/kodlari_gom.py
"""
import html
import pathlib
import re

root = pathlib.Path(__file__).resolve().parent.parent
page = root / "index.html"
text = page.read_text(encoding="utf-8")
pattern = re.compile(r'(<code[^>]*data-src="([^"]+)"[^>]*>)(.*?)(</code>)', re.S)
count = 0


def fill(match):
    global count
    count += 1
    code = (root / match.group(2)).read_text(encoding="utf-8").rstrip("\n")
    return match.group(1) + html.escape(code, quote=False) + match.group(4)


page.write_text(pattern.sub(fill, text), encoding="utf-8")
print(f"{count} kod gomuldu")
