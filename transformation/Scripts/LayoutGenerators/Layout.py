import math

class Layout:
    def __init__(self, name, refWidth = None, refHeight = None, rotation = None):
        self.name = name
        self.refWidth = refWidth if (refWidth is None or refWidth > 0) else None
        self.refHeight = refHeight if (refHeight  is None or refHeight > 0) else None
        self.rotation = rotation

    def GetName(self):
        return self.name

    def GetRefResolutionLine(self):
        if self.refWidth is not None and self.refHeight is not None:
            return 'refWidth={}\nrefHeight={}\n'.format(int(self.refWidth), int(self.refHeight))
        else:
            return ''

    def GetYawPitchRoll(self):
        if self.rotation is not None:
            (yaw, pitch, roll) = self.rotation.GetEulerAngles()
            return 'yaw={}\npitch={}\nroll={}\n'.format(math.degrees(yaw),
                math.degrees(pitch), math.degrees(roll))
        else:
            return ''

    def GenerateLayout(a = None):
        raise NotImplementedError( "Should have implemented this" )
