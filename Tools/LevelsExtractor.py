import sys
from xml.dom import minidom
from xml.etree.ElementTree import Element, SubElement
from xml.etree.ElementTree import tostring as xml_tostring

import xlrd

def isNumber(s):
    try:
        float(s) # for int, long and float
    except ValueError:
        try:
            complex(s) # for complex
        except ValueError:
            return False

    return True

def getEnemiesPool(levelIndex, document):
    sheet = document.sheet_by_name("LEVEL ENEMIEs")
    rowid = -1

    for y in range(0, sheet.nrows):
        cell = sheet.cell(y, 0)
        if cell.value == 'level':
            rowid = y
            break

    if rowid < 0:
        return ""

    pool = ""

    for y in range(rowid, sheet.nrows):
        idCell = sheet.cell(y, 0)
        if isNumber(idCell.value) and int(idCell.value) == levelIndex:
            for x in range(1, 6):
                enemyCell = sheet.cell(y, x)
                enemyId = enemyCell.value
                if not isNumber(enemyId) and len(enemyId) > 0:
                    pool += (', ' if x > 1 else '') +  enemyId
            break

    return pool

absPath = sys.argv[1]

xl_document = xlrd.open_workbook(absPath)
xl_sheet = xl_document.sheet_by_name("LEVEL progression")

# looking for beginning of levels description part

row_index = -1
for x in range(0, xl_sheet.nrows):
    xl_cell = xl_sheet.cell(x, 0)
    if xl_cell.value == 'level':
        row_index = x
        break

if row_index < 0:
    print('Failed to find levels description.')
    sys.exit(1)


# collecting levels data

class LevelInfo:
    def __init__(self):
        pass

    id = ''
    lengthInRows = 0
    enemiesAmount = 0
    runningSpeedBegin = 0.0
    runningSpeedEnd = 0.0
    enemiesDifficult = 0.0
    rewardCoins = 0
    rewardCrystal = ""
    dropChance = 0.0
    dropOnce = True


levels_info = []
readingColumnId = {"id": 0, "rows": 1, "enemiesAmount": 3, "difficult": 9, "speed_begin": 10,
                   "speed_end": 11, "rewardCoins": 12, "rewardCrystal": 13, "dropChance": 14, "dropOnce": 15}
row_index += 1  # move to to the next row

for idx_x in range(row_index, xl_sheet.nrows):
    id_xl_cell = xl_sheet.cell(idx_x, readingColumnId["id"])
    rows_xl_cell = xl_sheet.cell(idx_x, readingColumnId["rows"])
    amount_xl_cell = xl_sheet.cell(idx_x, readingColumnId["enemiesAmount"])
    difficult_cell = xl_sheet.cell(idx_x, readingColumnId["difficult"])
    speed_begin_cell = xl_sheet.cell(idx_x, readingColumnId["speed_begin"])
    speed_end_cell = xl_sheet.cell(idx_x, readingColumnId["speed_end"])
    reward_coins_cell = xl_sheet.cell(idx_x, readingColumnId["rewardCoins"])
    reward_crystal_cell = xl_sheet.cell(idx_x, readingColumnId["rewardCrystal"])
    reward_drop_chance = xl_sheet.cell(idx_x, readingColumnId["dropChance"])
    reward_drop_once = xl_sheet.cell(idx_x, readingColumnId["dropOnce"])

    info = LevelInfo()
    info.id = int(id_xl_cell.value)
    info.lengthInRows = rows_xl_cell.value
    info.enemiesAmount = amount_xl_cell.value
    info.runningSpeedBegin = speed_begin_cell.value
    info.runningSpeedEnd = speed_end_cell.value
    info.enemiesDifficult = difficult_cell.value
    info.rewardCoins = reward_coins_cell.value
    info.rewardCrystal = reward_crystal_cell.value
    info.dropChance = reward_drop_chance.value if reward_drop_chance.value != '' else "0"
    info.dropOnce = reward_drop_once.value == 'true'

    levels_info.append(info)


# writing levels data to xml

def prettify(elem):
    rough_string = xml_tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent='    ')


levels_amount = len(levels_info)
root = Element("Levels")

for index in range(0, levels_amount):
    info = levels_info[index]

    id_str = 'level_' + str(info.id)

    level = SubElement(root, 'Level')
    level.attrib['id'] = id_str
    level.attrib['status'] = 'unlocked' if index == 0 else 'locked'
    level.attrib['condition'] = 'classic'

    rewards = SubElement(level, 'Rewards', coins=str(info.rewardCoins))
    drop = SubElement(rewards, 'Drop', itemId=info.rewardCrystal,
                      dropChance=str(int(info.dropChance)), dropOnce='true' if info.dropOnce else 'false')

    unlocks = SubElement(level, 'Unlocks')
    if index < levels_amount - 1:
        unlockable = levels_info[index + 1]
        unlockable_id = 'level_' + str(unlockable.id)

        unlock = SubElement(unlocks, 'Unlock', id=unlockable_id)

    description = SubElement(level, 'Description',
                             {
                                 'enemiesAmount': str(int(info.enemiesAmount)),
                                 'lengthInSquares': str(int(info.lengthInRows)),
                                 'runningSpeedBegin': str(info.runningSpeedBegin),
                                 'runningSpeedEnd': str(info.runningSpeedEnd),
                                 'enemiesDifficultCoeff': str(info.enemiesDifficult),
                                 'availableEnemiesPool': getEnemiesPool(info.id, xl_document)
                             })

xml_file = open("levels.xml", "w")
xml_file.write(prettify(root))
xml_file.close()

print("Completed successfully!... " + str(len(levels_info)) + " levels processed")
