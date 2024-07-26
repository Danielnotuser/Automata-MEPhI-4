import random
import exrex

with open("gen.txt", 'w') as f:
    tls = random() % 3
    if (tls < 2):
        a = list(exrex.generate("^(tel|fax)\\:\\+[0-9]{11}(\\,\\+[0-9]{11})*;$"))
    else:
        a = list(exrex.generate("^sms\\:\\+[0-9]{11}(\\,\\+[0-9]{11})*;?body=[0-9a-zA-Z\\.\\,\\?\\!]{1,64}$"))
    f.write(*list)    
        