textToHash = "This is a sample text that shall be hashed by a hash function. It shouldn't really matter what it is so long as it's pretty long and proper to test the hash function, but it should work well and be fast in order to be selected as my preferred hash function!!!"

import time
import hashlib

hashFunctions = {
	"MD5": lambda m: hashlib.md5(m.encode()).hexdigest(),
	"SHA1": lambda m: hashlib.sha1(m.encode()).hexdigest(),
	"SHA256": lambda m: hashlib.sha256(m.encode()).hexdigest(),
	"SHA3-256": lambda m: hashlib.sha3_256(m.encode()).hexdigest(),
	"SHAKE128": lambda m: hashlib.shake_256(m.encode()).hexdigest(32),
	"BLAKE2b": lambda m: hashlib.blake2b(m.encode()).hexdigest(),
	"BLAKE2s": lambda m: hashlib.blake2s(m.encode()).hexdigest(),
}

samples = 200000

for h in hashFunctions:
	start = time.time()
	
	# do the hashes
	for i in range(samples):
		hashFunctions[h](textToHash)
	
	end = time.time()
	
	# result
	print(f"{h} took {(end - start) * 1000}ms")
