import math

class DistanceToQoEReader(object):
    def __init__(self, pathToFile):
        self.distToQoE = []
        self.layoutToId = dict()
        with open(pathToFile, 'r') as i:
            first = True
            for line in i:
                if line[0] == '%':
                    continue
                else:
                    if first:
                        first = False
                        for layout in line[:-1].split(' ')[1:]:
                            self.layoutToId[layout] = len(self.distToQoE)
                            self.distToQoE.append(dict())
                    else:
                        line = line.split(' ')
                        dist = float(line[0])
                        counter = 0
                        for QoE in line[1:]:
                            self.distToQoE[counter][dist] = float(QoE)
                            counter += 1

    def GetQoE(self, distance, layout):
        if layout not in self.layoutToId:
            print('{} not in the distance to QoE file'.format(layout))
            exit(1)
        if distance < 0 or distance > math.pi:
            print('{} is not a valid distance'.format(distance))
            exit(1)
        dist1 = -1
        dist2 = 4
        for dist in self.distToQoE[self.layoutToId[layout]]:
            if dist <= distance:
                dist1 = max(dist1, dist)
            else:
                dist2 = min(dist2, dist)
        if dist1 == -1:
            dist1 = dist2
        if dist2 == 4:
            dist2 = dist1
        QoE1 = self.distToQoE[self.layoutToId[layout]][dist1]
        QoE2 = self.distToQoE[self.layoutToId[layout]][dist2]
        slope = (QoE1-QoE2)/(dist1-dist2) if dist1 != dist2 else 0
        QoE = slope * distance + QoE1 - slope*dist1
        return QoE
