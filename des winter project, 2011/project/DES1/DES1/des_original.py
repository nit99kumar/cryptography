#! /usr/bin/python

import Image, sys, os, time, cProfile

PC1=[
        56, 48, 40, 32, 24, 16,  8,
	 0, 57, 49, 41, 33, 25, 17,
	 9,  1, 58, 50, 42, 34, 26,
	18, 10,  2, 59, 51, 43, 35,
	62, 54, 46, 38, 30, 22, 14,
	 6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28,
	20, 12,  4, 27, 19, 11,  3
    ]
    
PC2=[
        13, 16, 10, 23,  0,  4,
	 2, 27, 14,  5, 20,  9,
	22, 18, 11,  3, 25,  7,
	15,  6, 26, 19, 12,  1,
	40, 51, 30, 36, 46, 54,
	29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52,
	45, 41, 49, 35, 28, 31
    ]

IP=[
        57, 49, 41, 33, 25, 17, 9,  1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7,
	56, 48, 40, 32, 24, 16, 8,  0,
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6
    ]

E=[
        31,  0,  1,  2,  3,  4,
	 3,  4,  5,  6,  7,  8,
	 7,  8,  9, 10, 11, 12,
	11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20,
	19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28,
	27, 28, 29, 30, 31,  0
   ]
    
S1=[
        14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7, 
         0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8, 
         4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0, 
        15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
    ]
     
S2=[
        15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10, 
         3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5, 
         0, 14,  7, 11, 10, 14, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15, 
        13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
    ]

S3=[
        10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8, 
        13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1, 
        13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7, 
         1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
    ]
   
S4=[
         7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15, 
        13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9, 
        10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4, 
         3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
    ]
    
S5=[
         2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9, 
        14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6, 
         4,  2,  1, 11, 10, 13,  7,  8, 15,  9,  1,  5,  6,  3,  0, 14, 
        11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
    ]

S6=[
        12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11, 
        10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8, 
         9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6, 
         4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
    ]

S7=[
         4, 11,  2, 14, 15, 0,  8, 13, 3,  12, 9,  7,  5, 10, 6,  1, 
        13,  0, 11,  7,  4, 9,  1, 10, 14,  3, 5, 12,  2, 15, 8,  6, 
         1,  4, 11, 13, 12, 3,  7, 14, 10, 15, 6,  8,  0,  5, 9,  2, 
         6, 11, 13,  8,  1, 4, 10,  7,  9,  5, 0, 15, 14,  2, 3, 12
    ]

S8=[
        13,  2,  8, 4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7, 
         1, 15, 13, 8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2, 
         7, 11,  4, 1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8, 
         2,  1, 14, 7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
    ]

    
IPI=[
        39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25,
	32,  0, 40,  8, 48, 16, 56, 24
     ]

P=[
        15,  6, 19, 20,
        28, 11,	27, 16,
         0, 14, 22, 25,
	 4, 17, 30,  9,
         1,  7, 23, 13,
        31, 26,  2,  8,
	18, 12, 29,  5,
        21, 10,  3, 24
   ]

S=[S1, S2, S3, S4, S5, S6, S7, S8]

chk_list = [1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28]

def str_to_bin(string):
    ord_str_l=[ord(a) for a in string]
    l=len(ord_str_l)
    bin_str=[0]*(l<<3)
    for i in range(l):
        digit=ord_str_l[i]
        for j in range(8):
            bin_str[(i<<3)+j]=((digit&(1<<(7-j)))>>(7-j))
            
    return bin_str

def keygen(key, itr):
    c = key[:28]
    d = key[28:]
    c = c[chk_list[itr]:]+c[:chk_list[itr]]
    d = d[chk_list[itr]:]+d[:chk_list[itr]]
    e = c+d
    
    return permute(PC2, e)

def f_function(rext, k):
    string = xor(rext, k)
    l=[0]*32
    for i in range(8):
        m=(string[6*i]<<1)+(string[6*i+5])
        n=(string[6*i+1]<<3)+(string[6*i+2]<<2)+(string[6*i+3]<<1)+(string[6*i+4])
        digit=S[i][(m<<4)+n]
        l[i<<2]=(digit&8)>>3
        l[(i<<2)+1]=(digit&4)>>2
        l[(i<<2)+2]=(digit&2)>>1
        l[(i<<2)+3]=(digit&1)
            
    return permute(P, l)

def permute(table, list_to_per):
    l=len(table)
    per_table=[0]*l
    for i in range(l):
        per_table[i]=list_to_per[table[i]]
    return per_table

def xor(list1, list2):
    l=len(list1)
    new_list=[0]*l
    for i in range(l):
        new_list[i]=list1[i]^list2[i]

    return new_list

def des(plntext, key, IV, chk): #chk=0 for ecb else 1 
    plnbin=str_to_bin(plntext)
    key=str_to_bin(key)
   
    k=permute(PC1,key)

    if chk==1:
        plnbin1=xor(plnbin, IV)

    plnbin1=permute(IP,plnbin)

    for i in range(16):
        k2 = keygen(k, i)
        l=plnbin1[:32]
        r=plnbin1[32:]
        rext=permute(E,r)
        r_temp=f_function(rext, k2)
        l1=r
        r1=xor(r_temp, l)
        plnbin1=l1+r1
    
    enc_block=r1+l1
    
    return permute(IPI, enc_block)

def des_encrypt(plaintext, key, check):
    n, i=len(plaintext)/8, 0
    encrypted_l=[]
    if check==0:
        for i in range(n):
            encrypted_l+=(des(plaintext[(i<<3):((i+1)<<3)], key, '', check))
    else:
        iv=os.urandom(8)
        for i in range(n):
            if(i==0):
                encrypted_l+=(des(plaintext[(i<<3):((i+1)<<3)], key, str_to_bin(iv), check))
            else:
                encrypted_l+=(des(plaintext[(i<<3):((i+1)<<3)], key, encrypted_l[8*(i-1):], check))
    return encrypted_l
    
def bin2str(bin_list):
    n, i=len(bin_list)/8, 0
    s=['']*n
    for i in range(n):
        lis=bin_list[(i<<3):(i<<3)+8]
        digit=0
        for j in range(8):
            digit+=((lis[j])<<(7-j))
        s[i]=chr(digit)
    return ''.join(s)

def main():
    s=time.time()
    print des_encrypt('\x01#Eg\x89\xab\xcd\xef', '\x134Wy\x9b\xbc\xdf\xf1', 0)
    print time.time()-s
def rain():
    check=int(raw_input("Enter 0 for ECB else 1 for CBC - "))
    input_filename="test.jpg"
    input_image=Image.open(input_filename)
    plaintext=input_image.tostring()
    if len(plaintext)%8!=0:
        plaintext+='x'*(8-len(plaintext)%8)
    enc_img_bin = des_encrypt(plaintext, 'deskeyen', check)
    enc_img=bin2str(enc_img_bin)
    enc_img=Image.frombuffer("RGB",  input_image.size, enc_img, 'raw', 'RGB', 0, 1)
    enc_img.save("output.jpg", "JPEG")


if __name__ == "__main__":
    main()
