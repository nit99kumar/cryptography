import time
from Crypto.Cipher import AES
  
IV_SIZE = 16
BLOCK_SIZE = 16
  

  

   
  
# Key must be one of 16/24/32 bytes in length.
key = "0123456789ABCDEF"
mode = AES.MODE_CBC if cipher_mode == 'CBC' else AES.MODE_ECB
iv = os.urandom(IV_SIZE)

start=time.time()
aes = AES.new(key, mode, iv)
  
image_string= 'nitesh99'*10000   
  
# generate the encrypted image string
encrypted = aes.encrypt(image_string)

print time.time()-start
  
    

