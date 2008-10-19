option batch on
option confirm off
open
cd /var
option transfer binary
lcd e:\projects\tibiaauto-pub
put TibiaAutoSetup_BETA.exe
close
exit