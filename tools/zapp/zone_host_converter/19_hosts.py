# Encode EMTF host number, which is more narrowly defined than site number
# Total: 19 (9 from CSC + 9 from GEM & RPC + 1 from ME0)
# The LUT translates (type, station, ring) -> host
def find_emtf_host_initializer():
  default_value = -99
  lut = np.full((5,5,5), default_value, dtype=np.int32)
  lut[1,1,4] = 0  # ME1/1a
  lut[1,1,1] = 0  # ME1/1b
  lut[1,1,2] = 1  # ME1/2
  lut[1,1,3] = 2  # ME1/3
  lut[1,2,1] = 3  # ME2/1
  lut[1,2,2] = 4  # ME2/2
  lut[1,3,1] = 5  # ME3/1
  lut[1,3,2] = 6  # ME3/2
  lut[1,4,1] = 7  # ME4/1
  lut[1,4,2] = 8  # ME4/2
  lut[3,1,1] = 9  # GE1/1
  lut[2,1,2] = 10 # RE1/2
  lut[2,1,3] = 11 # RE1/3
  lut[3,2,1] = 12 # GE2/1
  lut[2,2,2] = 13 # RE2/2
  lut[2,2,3] = 13 # RE2/3
  lut[2,3,1] = 14 # RE3/1
  lut[2,3,2] = 15 # RE3/2
  lut[2,3,3] = 15 # RE3/3
  lut[2,4,1] = 16 # RE4/1
  lut[2,4,2] = 17 # RE4/2
  lut[2,4,3] = 17 # RE4/3
  lut[4,1,1] = 18 # ME0
  return lut
