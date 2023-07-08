#define _CRT_SECURE_NO_WARNINGS 1
#define BUFLEN 1024
#define MAXLINE 100000

#include <stdio.h>
#include <malloc.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <Python.h>
#include <windows.h>

struct tagLists {
	int idx;
	char tagName[400];
	char* engName;
	struct tagLists* next;
};

typedef struct tagLists taglist;
typedef struct tagLists* taglist_ptr;

const char FILENAME[2048] = "configOutput.json";
const char GETNAME[2048] = "restoreOutputAuctions.json";
const char DAYCOUNT_DATA[2048] = "maxDayCount";
const int rangeAmount = 5;
const int __DEBUG = 1;

char weapon[100];
int tagsOpp = 0;
taglist_ptr banWeapon;
taglist_ptr normalList;
int printTags = 0;
int moneyMin = -1;
int moneyMax = -1;
int dayCount = 2;

taglist_ptr initNewTag() {
	taglist_ptr newTag = (taglist_ptr)(malloc(sizeof(taglist)));
	newTag->idx = 0;
	newTag->next = NULL;
	return newTag;
};

char* getEngChar(taglist_ptr insertingList, int index) {
	int found = 0;
	while (insertingList != NULL) {
		if (insertingList->idx == index) {
			return insertingList->engName;
		}
		insertingList = insertingList->next;
	}

	return "found Nothing";
}


char* getChar(taglist_ptr insertingList, int index) {
	int found = 0;
	while (insertingList != NULL) {
		if (insertingList->idx == index) {
			return insertingList->tagName;
		}
		insertingList = insertingList->next;
	}

	return "found Nothing";
}

void endInsert(taglist_ptr insertingList, char* tag, char* eng) {
	taglist_ptr newTag = initNewTag();
	taglist_ptr headTag = insertingList;
	int countNum = 1;
	while (insertingList->next != NULL) {
		countNum++;
		insertingList = insertingList->next;
	}

	newTag->idx = countNum;
	strcpy(newTag->tagName, tag);
	newTag->engName = eng;
	insertingList->next = newTag;
	headTag->idx = countNum;
}

taglist_ptr getList() {
	taglist_ptr newTag = initNewTag();
	endInsert(newTag, "暴击率",			"critical_chance");
	endInsert(newTag, "暴击伤害",			"critical_damage");
	endInsert(newTag, "变焦",			"zoom");
	endInsert(newTag, "冰冻伤害",			"cold_damage");
	endInsert(newTag, "冲击伤害",			"impact_damage");
	endInsert(newTag, "触发几率",			"status_chance");
	endInsert(newTag, "触发时间",			"status_duration");
	endInsert(newTag, "穿刺伤害",			"puncture_damage");
	endInsert(newTag, "穿透",			"punch_through");
	endInsert(newTag, "弹匣容量",			"magazine_capacity");
	endInsert(newTag, "弹药上限",			"ammo_maximum");
	endInsert(newTag, "电击伤害",			"electric_damage");
	endInsert(newTag, "毒素伤害",			"toxin_damage");

	endInsert(newTag, "对Corpus伤害",	"damage_vs_corpus");
	endInsert(newTag, "对Grineer伤害",	"damage_vs_grineer");
	endInsert(newTag, "对Infested伤害",  "damage_vs_infested");
	endInsert(newTag, "多重射击",			"multishot");
	endInsert(newTag, "后坐力",			"recoil");
	endInsert(newTag, "火焰伤害",			"heat_damage");
	endInsert(newTag, "基础伤害",			"base_damage_/_melee_damage");
	endInsert(newTag, "切割伤害",			"slash_damage");

	endInsert(newTag, "射速/攻击速度",	"fire_rate_/_attack_speed");
	endInsert(newTag, "投掷物飞行速度",	"projectile_speed");
	endInsert(newTag, "装填速度",			"reload_speed");

	endInsert(newTag, "初始连击数",		"channeling_damage");
	endInsert(newTag, "处决伤害",			"finisher_damage");
	endInsert(newTag, "额外连击数获取",	"chance_to_gain_extra_combo_count");
	endInsert(newTag, "攻击范围",			"range");
	endInsert(newTag, "滑行攻击暴击率",	"critical_chance_on_slide_attack");
	endInsert(newTag, "连击持续时间",		"combo_duration");
	endInsert(newTag, "重击效率",			"channeling_efficiency");
	endInsert(newTag, "有负面词条", "has");
	endInsert(newTag, "无负面词条", "none");

	return newTag;
}



int endInsertValue(taglist_ptr insertingList, int value) {
	if (value >= 31) {
		return 0;
	}

	taglist_ptr newTag = initNewTag();
	taglist_ptr headTag = insertingList;
	int countNum = 1;
	int shouldInvert = 1;
	while (insertingList->next != NULL) {
		countNum++;
		insertingList = insertingList->next;
		if (value == insertingList->idx || value == tagsOpp) {
			shouldInvert = 0;
		}
	}
	
	if (shouldInvert == 1) {
		newTag->idx = value;
		strcpy(newTag->tagName, getChar(getList()->next, value));
		newTag->engName = getEngChar(getList()->next, value);
		insertingList->next = newTag;
		headTag->idx = countNum; 
	}

	return shouldInvert;
}




int endInsertChar(taglist_ptr insertingList) {
	char wpn[2024] = {'\0'};
	scanf("%s", wpn);
	taglist_ptr newTag = initNewTag();
	taglist_ptr topTag = insertingList;
	taglist_ptr headTag = insertingList;
	int countNum = 1;
	int shouldInvert = 1;
	while (headTag->next != NULL) {
		countNum++;
		headTag = headTag->next;
	}

	if (shouldInvert == 1) {
		newTag->idx = countNum;
		strcpy(newTag->tagName, wpn);
		newTag->engName = "none";
		headTag->next = newTag;
		topTag->idx = countNum;
	}

	return shouldInvert;
}

void printList(taglist_ptr insertingList) {
	insertingList = insertingList->next;
	int countNum = 1;
	while (insertingList != NULL) {
		countNum++;
		printf("  %2d:%-15s\t", insertingList->idx, insertingList->tagName);
		insertingList = insertingList->next;
		if ((countNum - 1) % 2 == 0) {
			printf("\n");
		}
	}
	printf("\n");
}

void printListTag(taglist_ptr insertingList) {
	insertingList = insertingList->next;
	while (insertingList != NULL) {
		char str2[20];

		strcpy(str2, insertingList->tagName);
		printf(" %d:%s; ", insertingList->idx, str2);
		insertingList = insertingList->next;
	}
	printf("\n");
}

void funcInit() {
	if (__DEBUG == 1)
		system("cls");

	printf("%s", "\n 总代号-词条表如下：\n");
	taglist_ptr mainList = getList();
	printList(mainList);
	printf(" 指令集:\n");
	printf("  %-22s:%-15s|%-22s:%-15s|%-22s:%-15s\n", "[/pos_add +代号]", "增加正面词条", "[/opp_add +代号]", "增加负面词条", "[/wpn_add +名字]", "修改武器名字");
	printf("  %-22s:%-15s|%-22s:%-15s|%-22s:%-15s\n", "[/set_money_min +数字]", "修改最小金钱", "[/set_money_min +数字]", "修改最大金钱", "[/monitor]", "开始监控");
	printf("  %-22s:%-15s|%-22s:%-15s|%-22s:%-15s\n", "[/pos_reset]", "重置正面词条", "[/opp_reset]", "重置负面词条", "[/wpn_reset]", "重置武器名字");
	printf("  %-22s:%-15s|%-22s:%-15s\n", "[/ban_wpn +名字]", "禁止武器", "[/unban_wpn +名字]", "解禁武器");
	printf("  %-22s:%-15s|%-22s:%-15s|%-22s:%-15s\n", "[/reset]", "重置所有", "[/exit]", "退出程序", "[/search]", "进行搜索");
	printf("  [武器留空即全武器]\n");
}

void delete(taglist_ptr list, char* value) {
	taglist_ptr preptr = list;
	list = list->next;
	taglist_ptr currentptr = list;
	while (currentptr != NULL) {
		if (strcmp(currentptr->tagName, value) == 0) {
			preptr->next = currentptr->next;
			free(currentptr);
			break;
		}

		preptr = preptr->next;
		currentptr = currentptr->next;
	}
}

void sprintTags() {
	if (printTags >= 1) {
		printf(" [已键入]正面词条:");
		printListTag(normalList);
	}
	if (tagsOpp != 0) {
		printf(" [已键入]负面词条: %d:%s;\n", tagsOpp, getChar(getList()->next, tagsOpp));
	}
	if (strlen(weapon) > 0) {
		printf(" [已键入]武器名字: %s;\n", weapon);
	}
	if (moneyMax >= 0 || moneyMin >= 0) {
		printf(" [已键入]价格区间: 最小:%d - 最大:%d;\n", moneyMin, moneyMax);
	}
}

int scanNormalList(value) {
	taglist_ptr head = normalList;
	head = head->next;
	int state = 0;
	while (head != NULL) {
		if (head->idx == value) {
			state = 1;
		}
		head = head->next;
	}

	return state;
}

void createJsonInfo() {
	taglist_ptr head = normalList->next;
	taglist_ptr bHead = banWeapon->next;
	cJSON* root = NULL;
	char* out = NULL;
	root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "type", "riven");
	cJSON_AddStringToObject(root, "polarity", "any");
	cJSON_AddStringToObject(root, "sort_by", "price_asc");
	cJSON* positive_stats = cJSON_CreateArray();
	cJSON* banWpns = cJSON_CreateArray();
	while (head) {
		cJSON_AddItemToArray(positive_stats, cJSON_CreateString(head->engName));
		head = head->next;
	}

	while (bHead) {
		cJSON_AddItemToArray(banWpns, cJSON_CreateString(bHead->tagName));
		bHead = bHead->next;
	}

	cJSON_AddItemToObject(root, "ban_wpns", banWpns);
	cJSON_AddItemToObject(root, "positive_stats", positive_stats);
	cJSON_AddStringToObject(root, "negative_stats", ((tagsOpp == 0) ? "NULL" : getEngChar(getList()->next, tagsOpp)));
	cJSON_AddStringToObject(root, "weapon_url_name", ((weapon[0] == '\0') ? "NULL" : weapon));
	cJSON_AddNumberToObject(root, "range_money_min", moneyMin);
	cJSON_AddNumberToObject(root, "range_money_max", moneyMax);
	cJSON_AddNumberToObject(root, "range_amount", rangeAmount);
	out = cJSON_Print(root);
	
	FILE* file = NULL;
	file = fopen(FILENAME, "w");
	if (file == NULL) {
		printf("Open file fail！\n");
		cJSON_Delete(root);
		return;
	}

	char* cjValue = cJSON_Print(root);
	int ret = fputs(cjValue, file);
	if (ret == EOF) {
		printf("写入文件失败！\n");
	}

	fclose(file);
	free(cjValue);
}

char* citoa(int num, char* str, int radix)
{
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum;
	int i = 0, j, k;

	if (radix == 10 && num < 0)
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else unum = (unsigned)num;

	do
	{
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;

	} while (unum);

	str[i] = '\0';

	if (str[0] == '-') k = 1;
	else k = 0;

	char temp;
	for (j = k; j <= (i - 1) / 2; j++)
	{
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}

	return str;

}

//void initBan_and_Counts() {
//	cJSON* cjson_test = NULL;
//	cJSON* cjson_banWpns = NULL;
//	int    wpns_array_size = 0, i = 0;
//	cJSON* cjson_banWpn = NULL;
//
//	FILE* file;
//	char buf[BUFLEN];
//	char* array[MAXLINE];
//	file = fopen(FILENAME, "r");
//	if (!file)return -1;
//	fgets(buf, BUFLEN, file);
//	fclose(file);
//
//	FILE* Dfile;
//	Dfile = fopen(DAYCOUNT_DATA, "rt");
//	char ch = fgetc(Dfile);
//	fclose(Dfile);
//
//	dayCount = (int)ch;
//
//	cjson_test = cJSON_Parse(buf);
//
//	cjson_banWpns = cJSON_GetObjectItem(cjson_test, "ban_wpns");
//	wpns_array_size = cJSON_GetArraySize(cjson_banWpns);
//	printf("%d", wpns_array_size);
//	for (i = 0; i < wpns_array_size; i++)
//	{
//		printf("123");
//		cjson_banWpn = cJSON_GetArrayItem(cjson_banWpns, i);
//		endInsert(banWeapon, cjson_banWpn->valuestring, "none");
//	}
//}

void getInput() {
	cJSON* cjson_test = NULL;
	cJSON* cjson_amount = NULL;
	cJSON* cjson_content = NULL;
	cJSON* cjson_owner = NULL;
	cJSON* cjson_weapon_name = NULL;
	cJSON* cjson_price = NULL;
	cJSON* cjson_opposite_name = NULL;
	cJSON* cjson_id = NULL;
	cJSON* cjson_name = NULL;
	char string[16] = { 0 };

	FILE* file;
	char buf[BUFLEN];
	int len = 0, i = 0;
	char* array[MAXLINE];

	file = fopen(GETNAME, "r");
	if (!file)return -1;

	fgets(buf, BUFLEN, file);

	fclose(file);
	cjson_test = cJSON_Parse(buf);
	while (cjson_test == NULL) {
		cjson_test = cJSON_Parse(buf);
	}

	cjson_amount = cJSON_GetObjectItem(cjson_test, "amount");

	for (int i = 0; i < rangeAmount; i++) {
		citoa(i, string, 10);
		cjson_content = cJSON_GetObjectItem(cjson_test, string);
		if (cjson_content != NULL) {
			cjson_owner = cJSON_GetObjectItem(cjson_content, "owner");
			cjson_weapon_name = cJSON_GetObjectItem(cjson_content, "weapon_name");
			cjson_opposite_name = cJSON_GetObjectItem(cjson_content, "opposite_name");
			cjson_price = cJSON_GetObjectItem(cjson_content, "price");
			cjson_name = cJSON_GetObjectItem(cjson_content, "name");
			cjson_id = cJSON_GetObjectItem(cjson_content, "id");
			printf("\n");
			printf("  %d:Mod名:%s - 武器名:%s - 价格:%d - 负面词条:%s\n", i + 1, cjson_name->valuestring, cjson_weapon_name->valuestring, cjson_price->valueint, cjson_opposite_name->valuestring);
			printf("   交易链接: https://warframe.market/auction/%s\n", cjson_id->valuestring);
			printf("   交易语句: /w %s hi! i want to buy your [%s %s] for %dpl\n", cjson_owner->valuestring, cjson_weapon_name->valuestring, cjson_name->valuestring, cjson_price->valueint);
		}
	}
}

void resetFunc() {
	normalList = initNewTag();
	printTags = 0;
	tagsOpp = 0;
	weapon[0] = '\0';
	moneyMax = -1;
	moneyMax = -1;
}

void pythonCommanding() {
	char* ret = "NULL";

	Py_Initialize();

	PyObject* pModule;
	PyObject* pFunction;
	PyObject* pArgs;
	PyObject* pRetValue;

	pModule = PyImport_ImportModule("getHttpCommand");
	if (!pModule) {
		PyErr_Print();
		printf("import python failed1!!\n");
		return -1;
	}

	pFunction = PyObject_GetAttrString(pModule, "main_func");
	if (!pFunction) {
		printf("get python function failed!!!\n");
		return -1;
	}

	pArgs = PyTuple_New(0);
	pRetValue = PyObject_CallObject(pFunction, pArgs);

	Py_Finalize();
}

int main()
{
	banWeapon = initNewTag();
	normalList = initNewTag();
	SetConsoleTitle(L"加拿大原神洋垃圾采集器");

	int stateExit = 0;
	char command[20];

	while (stateExit != 1) {
		funcInit();
		sprintTags();
		printf("\n");
		printf("%s", " 请输入指令:");

		scanf("%s", (command));

		if (strcmp(command, "/pos_add") == 0) {
			if (printTags + 1 <= 3) {
				int getNum = 0;
				funcInit();
				sprintTags();
				printf("\n");
				printf("%s", " [NUMBER] 请输入数字:");
				scanf("%d", (&getNum));

				int insertSuccess = endInsertValue(normalList, getNum);
				if (insertSuccess == 1)
					printTags++;
			}
		}
		if (strcmp(command, "/ban_wpn") == 0) {
			funcInit();
			sprintTags();
			printf("\n");
			printf("%s", " [NUMBER] 请输入武器名:");
			endInsertChar(banWeapon);
			printListTag(banWeapon);
		}
		if (strcmp(command, "/unban_wpn") == 0) {
			char wpn[2024];
			funcInit();
			sprintTags();
			printf("\n");
			printf("%s", " [NUMBER] 请输入武器名:");
			scanf("%s", wpn);
			delete(banWeapon, wpn);
		}
		else if (strcmp(command, "/wpn_add") == 0) {
			funcInit();
			sprintTags();
			printf("\n");
			printf("%s", " [NUMBER] 请输入武器名字:");
			scanf("%s", weapon);
		}
		else if (strcmp(command, "/opp_add") == 0) {
			funcInit();
			sprintTags();
			int getNum = 0;
			printf("\n");
			printf("%s", " [NUMBER] 请输入数字:");
			scanf("%d", (&getNum));

			if (scanNormalList(getNum) == 0) {
				tagsOpp = getNum;
			}	
		}
		else if (strcmp(command, "/set_money_min") == 0) {
			funcInit();
			sprintTags();
			printf("\n");
			printf("%s", " [NUMBER] 请输入最小价格:");
			scanf("%d", (&moneyMin));
		}
		else if (strcmp(command, "/set_money_max") == 0) {
			funcInit();
			sprintTags();
			printf("\n");
			printf("%s", " [NUMBER] 请输入最大价格:");
			scanf("%d", (&moneyMax));
		}
		else if (strcmp(command, "/exit") == 0) {
			stateExit = 1;
		}
		else if (strcmp(command, "/pos_reset") == 0) {
			normalList = initNewTag();
			printTags = 0;
		}
		else if (strcmp(command, "/opp_reset") == 0) {
			tagsOpp = 0;
		}
		else if (strcmp(command, "/wpn_reset") == 0) {
			weapon[0] = '\0';
		}
		else if (strcmp(command, "/reset") == 0) {
			resetFunc();
		}
		else if (strcmp(command, "/search") == 0) {
			createJsonInfo();
			resetFunc();
			pythonCommanding();
			Sleep(3000);
			getInput();
			scanf("%s", command);
		}
		else if (strcmp(command, "/monitor") == 0) {
			createJsonInfo();
			resetFunc();
			while (1) {
				pythonCommanding();
				Sleep(1000);
				funcInit();
				sprintTags();
				printf("\n 监控中\n");
				getInput();
			}
		}
	}

	return 0;
}