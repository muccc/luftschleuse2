import os
key = [ord(x) for x in os.urandom(16)]

print ' '.join([str(x) for x in key])
print ', '.join([str(x) for x in key])

