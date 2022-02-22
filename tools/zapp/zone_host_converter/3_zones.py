# Assign EMTF zones
# Zone 0 receives 9 hosts
# Zone 1 receives 10 hosts
# Zone 2 receives 8 hosts
# The LUT translates (host,zone) -> (min_theta,max_theta)
def find_emtf_zones_initializer():
  default_value = -99
  lut = np.full((19,3,2), default_value, dtype=np.int32)
  lut[0,0] = 4,26    # ME1/1
  lut[3,0] = 4,25    # ME2/1
  lut[5,0] = 4,25    # ME3/1
  lut[7,0] = 4,25    # ME4/1
  lut[9,0] = 17,26   # GE1/1
  lut[12,0] = 7,25   # GE2/1
  lut[14,0] = 4,25   # RE3/1
  lut[16,0] = 4,25   # RE4/1
  lut[18,0] = 4,23   # ME0
  lut[0,1] = 24,53   # ME1/1
  lut[1,1] = 46,54   # ME1/2
  lut[3,1] = 23,49   # ME2/1
  lut[5,1] = 23,41   # ME3/1
  lut[6,1] = 44,54   # ME3/2
  lut[7,1] = 23,35   # ME4/1
  lut[8,1] = 38,54   # ME4/2
  lut[9,1] = 24,52   # GE1/1
  lut[10,1] = 52,56  # RE1/2
  lut[12,1] = 23,46  # GE2/1
  lut[14,1] = 23,36  # RE3/1
  lut[15,1] = 40,52  # RE3/2
  lut[16,1] = 23,31  # RE4/1
  lut[17,1] = 35,52  # RE4/2
  lut[1,2] = 52,88   # ME1/2
  lut[4,2] = 52,88   # ME2/2
  lut[6,2] = 50,88   # ME3/2
  lut[8,2] = 50,88   # ME4/2
  lut[10,2] = 52,84  # RE1/2
  lut[13,2] = 56,88  # RE2/2
  lut[15,2] = 48,84  # RE3/2
  lut[17,2] = 52,84  # RE4/2
  return lut
