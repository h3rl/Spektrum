
import json

planet_names = ["Sun","Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune","Pluto"]

datatypes = dict()
datatypes["uint"] = 12

def datatypefor(a, b):
    if a < 0:
        if a >= -pow(2,7) and b <= pow(2,7)-1:
            return "int8_t"
        if a >= -pow(2,15) and b <= pow(2,15)-1:
            return "int16_t"
        if a >= -pow(2,31) and b <= pow(2,31)-1:
            return "int32_t"
        if a >= -pow(2,63) and b <= pow(2,63)-1:
            return "int64_t"
        pass
    else:
        if b <= pow(2,8)-1:
            return "uint8_t"
        if b <= pow(2,16)-1:
            return "uint16_t"
        if b <= pow(2,32)-1:
            return "uint32_t"
        if b <= pow(2,64)-1:
            return "uint64_t"

def getVal(planet, key):

    if key in ["mass", "vol"]:
        if planet[key] != None:
            v = planet[key][key+"Value"]
            e = planet[key][key+"Exponent"]
            val = int(v) * pow(10,int(e))
            return val

    try:
        val = float(planet[key])
        return val
    except:
        return None

with open("planetsraw.json") as file:

    data = json.load(file)

    keys = data["bodies"][0].keys()

    for key in keys:

        min_ = 0
        max_ = 0
        for planet in data["bodies"]:
            val = getVal(planet,key)
            if not val:
                break
            min_ = min(min_,val)
            max_ = max(max_,val)
        if min_ == 0 and max_ == 0:
            continue

        dtype = datatypefor(min_,max_)
        a = min_
        b = max_
        k = key
        
        print(f"{dtype} {key}{'{ 0 };'} {max_}")
    exit()

    for planet in data["bodies"]:
        name = planet["englishName"]

        for key in planet.keys():
            
            if key in ["mass","vol"]:
                pass
            else:
                print(f"{key} {planet[key]}")

        break
'''
id
name       
englishName
isPlanet   
moons      
semimajorAxis
perihelion
aphelion
eccentricity
inclination
mass
vol
density
gravity
escape
meanRadius
equaRadius
polarRadius
flattening
dimension
sideralOrbit
sideralRotation
aroundPlanet
discoveredBy
discoveryDate
alternativeName
axialTilt
avgTemp
mainAnomaly
argPeriapsis
longAscNode
bodyType
rel
'''