
print ("我叫 %s 今年 %d 岁!" % ('小明', 10))

list1 = ["1","2","3","4"]
print(list1[2])

it = iter(list1)
for x in it :
    print(x,end = ",")

print("\n不定长参数:")

def printinfo(arg1,*args):
    "print all args"  #么发现有球用
    print(arg1)
    print(arg1)
    print(args)
    print(args[1])

printinfo(1,2,3,4,5)

print("lambda:")

result = lambda a,b:a+b

print(result(3,4))

print("模块：")
import sys
for i in sys.argv:
    print(i)
sys.ps1