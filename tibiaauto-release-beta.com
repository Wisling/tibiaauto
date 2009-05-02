option batch on
option confirm off
open
cd /var
option transfer binary
lcd E:\Tibia\Sourse Code\tibiaauto-pub
put TibiaAutoSetup_BETA.exe
close
exit