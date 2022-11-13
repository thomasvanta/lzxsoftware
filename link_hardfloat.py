#!python
Import("env")

#print(env.Dump())

old_flags = env["CCFLAGS"]
old_flags = [x for x in old_flags if x != "-mfloat-abi=softfp"]
old_flags = [x for x in old_flags if x != "-mfloat-abi=soft"]
env["CCFLAGS"] = old_flags
old_flags = env["LINKFLAGS"]
old_flags = [x for x in old_flags if x != "-mfloat-abi=softfp"]
old_flags = [x for x in old_flags if x != "-mfloat-abi=soft"]
env["LINKFLAGS"] = old_flags
flags = [
   "-mfloat-abi=hard",
   "-mfpu=fpv4-sp-d16"
]
env.Append(CCFLAGS=flags, LINKFLAGS=flags)
