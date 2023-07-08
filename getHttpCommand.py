import requests
import json
import datetime

url = "https://api.warframe.market/v1/auctions/search"
restore = "restoreOutputAuctions.json"

def main_func():
    f = open('configOutput.json', 'r')
    content = f.read()
    outputJson = json.loads(content)

    inputList = {}
    outputPlayer = {}

    needRanging = True
    rangeCounting = 0;

    minMoney = outputJson["range_money_min"]
    maxMoney = outputJson["range_money_max"]
    rangeAmount = outputJson["range_amount"]

    banWpns = outputJson["ban_wpns"]

    if minMoney == -1 :
        minMoney = -999999

    if maxMoney == -1 :
        maxMoney = 999999

    if rangeAmount == 0 :
        needRanging = False

    if outputJson["negative_stats"] != "NULL":
        inputList["negative_stats"] = outputJson["negative_stats"]
    
    if outputJson["weapon_url_name"] != "NULL":
        inputList["weapon_url_name"] = outputJson["weapon_url_name"]
    
    if len(outputJson["positive_stats"]) > 0:
        inputList["positive_stats"] = ""
        count = 1
        while(count <= len(outputJson["positive_stats"])):
            inputList["positive_stats"] = inputList["positive_stats"] + outputJson["positive_stats"][count-1]
            if (count != len(outputJson["positive_stats"])):
                inputList["positive_stats"] = inputList["positive_stats"] + ","
            count = count +1

    inputList["type"] = outputJson["type"]
    inputList["polarity"] = outputJson["polarity"]
    inputList["sort_by"] = outputJson["sort_by"]

    Headers={
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36",
        "content-type": "application/json", 
        "accept": "application/json"
    }
    
    try:
        response = requests.get(url = url, params=inputList, headers = Headers)
        responseJson = json.loads(response.text)["payload"]["auctions"]
    except :
        print("  [python] 运行时出错")


    for players in responseJson:
        date = players["created"].split("T")[0]
        year = int(date.split("-")[0])
        month = int(date.split("-")[1])
        day = int(date.split("-")[2])
        dt = datetime.datetime(year, month, day)
        now = datetime.datetime.now()
        duration = dt - now

        if (players["buyout_price"] > minMoney and players["buyout_price"] < maxMoney and players["is_direct_sell"] == True and players["owner"]["status"] != "offline" and players["item"]["weapon_url_name"] not in banWpns and abs(duration.days) < 2):
            if (needRanging == False or rangeCounting <= rangeAmount - 1):
                opp_tag_name = ""

                for tags in players["item"]["attributes"]:
                    if tags["positive"] == False:
                        opp_tag_name = tags["url_name"]

                inserting = {}
                inserting["opposite_name"] = opp_tag_name
                inserting["weapon_name"] = players["item"]["weapon_url_name"]
                inserting["name"] = players["item"]["name"]
                inserting["price"] = players["buyout_price"]
                inserting["owner"] = players["owner"]["ingame_name"]
                inserting["id"] = players["id"]
                outputPlayer[rangeCounting] = inserting
                rangeCounting = rangeCounting + 1;


    outputPlayer["amount"] = rangeAmount
    file = open(restore, "w")
    json.dump(outputPlayer, file)
    file.close()


    