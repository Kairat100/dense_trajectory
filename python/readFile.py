f = open('test.txt', 'r')

line = f.readline()
arr = line.split()
l = int(arr[0])

numbers = []
while True:
    line = f.readline()
    if not line: break
    words = line.split()
    
    num = range(l*2+3)
    i = 0
    for x in words:
        num[i] = float(x)
        i+=1

    num[0] -= l
    numbers.append(num)

print l
print numbers[0]

#for 