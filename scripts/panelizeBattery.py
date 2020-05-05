from kikit import panelize
import shapely
import pcbnew
import sys

tabWidth = panelize.fromMm(65)
slotWidth = panelize.fromMm(2.5)
panelOrigin = pcbnew.wxPointMM(150, 40)

panel = panelize.Panel()

pos1 = panel.appendBoard(sys.argv[1], panelOrigin - pcbnew.wxPointMM(1.5, 0),
    origin=panelize.Origin.TopRight, tolerance=panelize.fromMm(5))
pos2 = panel.appendBoard(sys.argv[1], panelOrigin + pcbnew.wxPointMM(1.5, 0),
    origin=panelize.Origin.BottomRight, tolerance=panelize.fromMm(5),
    rotationAngle=1800)

tabs, cuts = panel.layerToTabs("Cmts.User", panelize.fromMm(5))
panel.appendSubstrate(tabs)
panel.addMillFillets(panelize.fromMm(1))

panel.makeMouseBites(cuts, panelize.fromMm(0.5), panelize.fromMm(0.75))

panel.save(sys.argv[2])
