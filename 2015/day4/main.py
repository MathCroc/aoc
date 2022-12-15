import hashlib

input = "bgvyzdsv"

suffix = 1
while True:
    s = input + str(suffix)
    out = hashlib.md5(s.encode('utf-8')).hexdigest()
    if out.startswith("00000"):
        print(suffix)
        break
    suffix += 1