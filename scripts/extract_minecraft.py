from zipfile import ZipFile
import os, json

RES_DIR = "res/minecraft"

with ZipFile(os.path.expanduser("~/.minecraft/versions/1.18.2/1.18.2.jar")) as z:
    for fn in json.load(open(os.path.join(RES_DIR, "extract_minecraft.json"))):
        if type(fn) is str:
            with open(os.path.join(RES_DIR, fn + ".png"), "wb+") as f:
                f.write(z.read(f"assets/minecraft/textures/block/{fn}.png"))
        
        elif type(fn) is dict:
            op = os.path.join(RES_DIR, fn["name"] + ".png")
            with open(op, "wb+") as f:
                f.write(z.read(f"assets/minecraft/textures/block/{fn.get('file', fn['name'])}.png"))
            if fn.get("tint", False):
                os.system(f"python scripts/tint.py {op} {op}")
            if fn.get("overlay") is not None:
                ov = fn["overlay"]
                ovf = os.path.join(RES_DIR, "OVERLAY" + ov["file"] + ".png")
                with open(ovf, "wb+") as f:
                    f.write(z.read(f"assets/minecraft/textures/block/{ov['file']}.png"))
                if ov.get("tint", False):
                    os.system(f"python scripts/tint.py {ovf} {ovf}")
                os.system(f"python scripts/overlay.py {op} {ovf} {op}")
                os.remove(ovf)
