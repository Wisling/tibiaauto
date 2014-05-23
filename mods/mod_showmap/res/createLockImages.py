import Image, os, re
jn = os.path.join
im_lock = Image.open(r"C:\Mark\Software\Cygwin\home\Mark The Mighty\tibia\tibiaauto-pub\mods\mod_showmap\res\map_lock.bmp")
im_lock_mask = Image.open(r"C:\Mark\Software\Cygwin\home\Mark The Mighty\tibia\tibiaauto-pub\mods\mod_showmap\res\map_lock_mask.bmp")
fdir = r"C:\Mark\Software\Cygwin\home\Mark The Mighty\tibia\tibiaauto-pub\mods\mod_showmap\res"
for fname in os.listdir(fdir):
    if fname in ["map_empty.bmp","map_self.bmp"]: break
    fpath = jn(fdir,fname)
    if re.match(r"map_.*\.bmp",fname) and not("lock.bmp" in fname) and (1 or os.path.exists(jn(fdir,fname[:-4]+"_lock.bmp"))):
        im = Image.open(fpath)
        im.paste(im_lock,(0,0,20,20),im_lock_mask)
        im.save(jn(fdir,fname[:-4]+"_lock.bmp"))
        