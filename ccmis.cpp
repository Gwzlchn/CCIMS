#include "ccmis.h"
#include "aboutui.h"

//跨平台判断
#ifdef Q_OS_WIN32
    const unsigned int CCMIS::PLATFORM_JUDGE = 0;
#elif defined (Q_OS_MAC)
    const unsigned int CCMIS::PLATFORM_JUDGE = 1;
#else
    const unsigned int CCMIS::PLATFORM_JUDGE = 2;
#endif

//自定义常量
const string CCMIS::BACKBUTTON_PICTURE_NAME = "BackButton.png";

const string CCMIS::OUT_FILE_NAME   = "user.csv";
const string CCMIS::USER_FILE_NAME  = "user.json";
const string CCMIS::SHOP_FILE_NAME  = "shop.json";
const string CCMIS::INFO_FILE_NAME  = "info.json";

const string CCMIS::JSON_KEY_NUMBER  = "number";
const string CCMIS::JSON_KEY_NAME  = "name";
const string CCMIS::JSON_KEY_PASSWORD  = "password";
const string CCMIS::JSON_KEY_BALANCE  = "balance";
const string CCMIS::JSON_KEY_COUPON  = "coupon";

const string CCMIS::JSON_KEY_TAG = "tag";
const string CCMIS::JSON_KEY_YEAR = "year";
const string CCMIS::JSON_KEY_MONTH = "month";
const string CCMIS::JSON_KEY_DAY = "day";
const string CCMIS::JSON_KEY_HOUR = "hour";
const string CCMIS::JSON_KEY_MINUTE = "minute";
const string CCMIS::JSON_KEY_SECOND = "second";
const string CCMIS::JSON_KEY_ONUMBER = "onumber";
const string CCMIS::JSON_KEY_INUMBER = "inumber";
const string CCMIS::JSON_KEY_MONEY = "money";

const int CCMIS::SUPERUSER_BEGIN = 0;
const int CCMIS::SUPERUSER_END = 999;
const int CCMIS::SHOP_CANTEEN_BEGIN = 1101;
const int CCMIS::SHOP_CANTEEN_END = 1999;
const int CCMIS::SHOP_MARKET_BEGIN = 2101;
const int CCMIS::SHOP_MARKET_END = 2999;
const int CCMIS::SHOP_BATH_BEGIN = 3101;
const int CCMIS::SHOP_BATH_END = 3999;
const int CCMIS::SHOP_BEGIN = 1000;
const int CCMIS::SHOP_END = 3999;
const int CCMIS::USER_TEA_EMP_BEGIN = 5000;
const int CCMIS::USER_TEA_EMP_END = 6999;
const int CCMIS::USER_BEGIN = 4000;
const int CCMIS::USER_END = 6999;

const int CCMIS::MESSAGE_TRANSACTION_SUCCESS = 0;
const int CCMIS::MESSAGE_TRANSACTION_NO_USER = -1;
const int CCMIS::MESSAGE_TRANSACTION_NO_SHOP = -2;
const int CCMIS::MESSAGE_TRANSACTION_OVERFLOW = -3;
const int CCMIS::MESSAGE_TRANSACTION_BALANCE_NOT_ENOUGH = -4;
const int CCMIS::MESSAGE_TRANSACTION_MONEY_LOWER_THAN_ZERO = -5;
const int CCMIS::MESSAGE_TRANSACTION_UNKNOWN = -6;

const int CCMIS::GROUP_SUPERUSER = 0;
const int CCMIS::GROUP_CANTEEN = 1;
const int CCMIS::GROUP_MARKET = 2;
const int CCMIS::GROUP_BATH = 3;

const int CCMIS::THREAD_TYPE_R_INFO = -1;
const int CCMIS::THREAD_TYPE_R_SHOP = -2;
const int CCMIS::THREAD_TYPE_R_USER = -3;

const int CCMIS::THREAD_TYPE_W_INFO = 1;
const int CCMIS::THREAD_TYPE_W_SHOP = 2;
const int CCMIS::THREAD_TYPE_W_USER = 3;

CCMIS::CCMIS()
{
    //初始化
    mUserNumber = -1;
    mReadThreadCount = 0;
    mWriteThreadCount = 0;

    mInfo = new Information();
    mShop = new Shop();
    mUser = new User();

    mInfo->next = NULL;
    mShop->next = NULL;
    mUser->next = NULL;

    totalInfoCount = 0;
    totalShopCount = 0;
    totalUserCount = 0;

    GuessTotalNumber = 5000; //预计5000

    JsonThread* jthread = new JsonThread(this, THREAD_TYPE_R_INFO);

    //读文件线程开启
    jthread->start();   //TODO: Need to send message when finished.

    if (!ReadUser(USER_FILE_NAME))  //判断打开文件是否成功
    {
        cout << "Open " + USER_FILE_NAME + " failed.";
    }

    if (!ReadShop(SHOP_FILE_NAME))
    {
        cout << "Open " + SHOP_FILE_NAME + " failed.";
    }


    //WriteUser("testuser.json");

    //GenerateTag(6998, 1101, 1500);

    //InsertInf(BuildInfo(6998, 1101, 1500));
    //WriteInf(INFO_FILE_NAME);

//    if (!ReadInf(INFO_FILE_NAME))
//    {
//        cout << "Open " + INFO_FILE_NAME + " failed.";
//    }

//    Information* test  = SearchInfoByInum(1201);
//    WriteInf("3216.json",test);

//    cout<<GetTotalCanteenConsumptionByDay(2018,1,3,4001)<<endl;
//    cout<<GetUserByNum(4000)<<endl;

//    cout << "start transaction...\n";
//    time_t timep;
//    time (&timep);
//    char timestr[64];
//    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S",localtime(&timep));
//    cout<<"start: "<<timestr<<endl;
//    cout << NewTransaction(4001, 3101, 2500)<<endl;

//    time (&timep);
//    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S",localtime(&timep));
//    cout<<"end: "<<timestr<<endl;

//    cout << "transaction complete...\n";
}

int CCMIS::GetCurrentReadThreadCount()
{
    return mReadThreadCount;
}

int CCMIS::GetCurrentWriteThreadCount()
{
    return mWriteThreadCount;
}

jsonxx::Array CCMIS::LinkListToJson(User* user_list)
{
    jsonxx::Array array;

    User* u = user_list->next;
    while (u != NULL) {
        jsonxx::Object obj;
        obj << JSON_KEY_NUMBER << u->number;
        obj << JSON_KEY_NAME << COMMON_FUNCS::ToUTF8String(u->name);
        obj << JSON_KEY_PASSWORD << u->password;
        obj << JSON_KEY_BALANCE << u->balance;
        obj << JSON_KEY_COUPON << u->coupon;
        array << obj;
        u=u->next;
    }
    return array;
}

jsonxx::Array CCMIS::LinkListToJson(Information* info_list)
{
    jsonxx::Array array;

    Information* i = info_list->next;
    while (i != NULL) {
        jsonxx::Object obj;

        obj << JSON_KEY_TAG << i->tag;
        obj << JSON_KEY_YEAR << i->year;
        obj << JSON_KEY_MONTH << i->month;
        obj << JSON_KEY_DAY << i->day;
        obj << JSON_KEY_HOUR << i->hour;
        obj << JSON_KEY_MINUTE << i->minute;
        obj << JSON_KEY_SECOND << i->second;
        obj << JSON_KEY_ONUMBER << i->Onumber;
        obj << JSON_KEY_INUMBER << i->Inumber;
        obj << JSON_KEY_MONEY << i->money;
        array << obj;
        i=i->next;
    }
    return array;
}

jsonxx::Array CCMIS::LinkListToJson(Shop* shop_list)
{
    jsonxx::Array array;

    Shop* s = shop_list->next;
    while (s != NULL) {
        jsonxx::Object obj;

        obj << JSON_KEY_NUMBER << s->number;
        obj << JSON_KEY_NAME << s->name;
        obj << JSON_KEY_PASSWORD << s->password;
        array << obj;
        s=s->next;
    }
    return array;
}

bool CCMIS::SaveJsonArrToFile(const jsonxx::Array& ToSaveJson,string filename)
{
    QString allJsonArray = QString::fromLocal8Bit(ToSaveJson.json().data());

    QFile fileOut(COMMON_FUNCS::UTF8ToQString(filename));
    if(!fileOut.open(QIODevice::WriteOnly|QIODevice::Text)){
        qDebug("Could not open the file for WRITING! \n");
        return false;
    }

    QTextStream streamFileOut(&fileOut);
    streamFileOut.setCodec("UTF-8");
    streamFileOut << allJsonArray;
    streamFileOut.flush();
    fileOut.close();
    return true;
}



//应该用多态用模板，下面这六个函数，怕吕帅而已。。。
bool CCMIS::WriteShop(string filename,Shop* shop_list)
{
    jsonxx::Array shop_json;
    if(shop_list == NULL)
        shop_json = LinkListToJson(mShop);
    else
        shop_json = LinkListToJson(shop_list);

    if(SaveJsonArrToFile(shop_json,filename))
        return true;
    else
        return false;
}

bool CCMIS::WriteInf(string filename,Information* info_list)
{

    jsonxx::Array info_json;
    if(info_list == NULL)
        info_json = LinkListToJson(mInfo);
    else
        info_json = LinkListToJson(info_list);

    if(SaveJsonArrToFile(info_json,filename))
        return true;
    else
        return false;
}


bool CCMIS::WriteUser(string filename,User* user_list)
{
    jsonxx::Array user_json;
    if(user_list == NULL)
        user_json = LinkListToJson(mUser);
    else
        user_json = LinkListToJson(user_list);

    if(SaveJsonArrToFile(user_json,filename))
        return true;
    else
        return false;
}


void CCMIS::WriteInfForDel()
{

    JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
    jthread->start();
    jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
    jthread->start();
}


QString CCMIS::FilenameCorrect(string filename)
{
    //跨平台处理
    QString finalfilename;
    switch (PLATFORM_JUDGE) {
    case 0:
        finalfilename = QString::fromStdString(filename);
        break;
    case 1:
        finalfilename = QCoreApplication::applicationDirPath();
        finalfilename.chop(24);
        finalfilename += QString::fromStdString(filename);
        break;
    default:
        finalfilename = QString::fromStdString(filename);
        qDebug("Could not run in this platform \n");
        break;
    }
    return finalfilename;
}


string CCMIS::ReadAllFileToQString(string filename)
{
    //读取文件
    QFile fileReadIn(FilenameCorrect(filename));
    if(!fileReadIn.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug ("Could not open the file for reading /n");
        return string("");
        //String("").Empty();  //结果为true判定用
    }

    QString allLine;
    QTextStream readInSteam(&fileReadIn);
    readInSteam.setCodec("UTF-8");
    while (!readInSteam.atEnd()) {
        QString line = readInSteam.readLine();
        line += '\n';
        allLine +=line;
    }

    fileReadIn.close();
    string  cstr = allLine.toStdString();
    return cstr;
}


bool CCMIS::ReadUser(string filename)
{
    /**by liuvv*/
    //change by wangzl using qfile, coding with utf-8
    string fileALLReadIn = ReadAllFileToQString(filename);
    if (!fileALLReadIn.empty()){
        jsonxx::Array array;

        //解析 json
        array.parse(fileALLReadIn);
        for (size_t i = 0; i < array.size(); i++)  //迭代构造
        {
            User* u = new User();

            u->number = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_NUMBER);
            u->name = COMMON_FUNCS::UTF8ToQString
                    (array.get<jsonxx::Object>(i).
                     get<jsonxx::String>(JSON_KEY_NAME));
            u->password = array.get<jsonxx::Object>(i).
                    get<jsonxx::String>(JSON_KEY_PASSWORD);
            u->balance = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_BALANCE);
            u->coupon = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_COUPON);

            /**DON NOT DELETE THE ANNOTATION RASHLY!!!*/
            /*u->balance = (rand() % (100000 - 0 + 1)) + 0;
            if (u->number < 5000)
            {
                u->coupon = (rand() % (10000 - 0 + 1)) + 0;
            } else {
                u->coupon = 0;
            }*/

            u->next = mUser->next;
            mUser->next = u;

            totalUserCount++;   //统计用户数
        }

        //验证数据
        User* u = mUser->next;
        while (u != NULL) {
            cout<<u->number<<"\t"<<u->name<<"\t"<<u->password
               <<"\t"<<u->balance<<"\t"<<u->coupon<<endl;
            u=u->next;
        }

        return true;
    } else {
        return false;
    }
}

bool CCMIS::ReadShop(string filename)
{
    /**by liuvv*/
    //change by wangzl using qfile, coding with utf-8
    string fileALLReadIn = ReadAllFileToQString(filename);
    if (!fileALLReadIn.empty()){
        jsonxx::Array array;

        //解析 json
        array.parse(fileALLReadIn);
        for (unsigned long i = 0; i < array.size(); i++)  //迭代构造
        {
            Shop* s = new Shop();

            s->number = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_NUMBER);
            s->name = COMMON_FUNCS::UTF8ToQString
                    (array.get<jsonxx::Object>(i).
                     get<jsonxx::String>(JSON_KEY_NAME));
            s->password = array.get<jsonxx::Object>(i).
                    get<jsonxx::String>(JSON_KEY_PASSWORD);

            s->next = mShop->next;
            mShop->next = s;


            totalShopCount++;   //统计商铺数
        }

        //验证数据
        Shop* sh = mShop->next;
        while (sh != NULL) {
            cout<<sh->number<<"\t"<<sh->name<<"\t"<<sh->password<<endl;
            sh=sh->next;
        }

        return true;
    } else {
        return false;
    }
}

bool CCMIS::ReadInf(string filename)
{
    string fileALLReadIn = ReadAllFileToQString(filename);

    if (!fileALLReadIn.empty()){
        jsonxx::Array array;
        array.parse(fileALLReadIn);             //解析 json
        for (unsigned long i = 0; i < array.size(); i++)  //迭代构造
        {
            Information* info = new Information();

            info->tag = array.get<jsonxx::Object>(i).
                    get<jsonxx::String>(JSON_KEY_TAG);
            info->year = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_YEAR);
            info->month = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_MONTH);
            info->day = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_DAY);
            info->hour = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_HOUR);
            info->minute = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_MINUTE);
            info->second = array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_SECOND);
            info->Inumber= array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_INUMBER);
            info->Onumber= array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_ONUMBER);
            info->money= array.get<jsonxx::Object>(i).
                    get<jsonxx::Number>(JSON_KEY_MONEY);

            InsertInf(info);
        }
    }

        //验证数据
        /*Information* info = mInfo->next;
        while (info != NULL) {
            cout
                 <<info->tag<<"\t"
                 <<info->year<<"\t"
                 <<info->month<<"\t"
                 <<info->day<<"\t"
                 <<info->hour<<"\t"
                 <<info->minute<<"\t"
                 <<info->second<<"\t"
                 <<info->Inumber<<"\t"
                 <<info->Onumber<<"\t"
                 <<info->money<<endl;
            info=info->next;
        }*/

        return true;

//    if (!fileALLReadIn.empty()){
//        jsonxx::Array array;
//        array.parse(fileALLReadIn);             //解析 json
//        for (unsigned long i = 0; i < array.size(); i++)  //迭代构造
//        {
//            //构造信息
//            Information* info = new Information();
//            info->tag = array.get<jsonxx::Object>(i).
//                    get<jsonxx::String>(JSON_KEY_TAG);
//            info->year = array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_YEAR);
//            info->month = array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_MONTH);
//            info->day = array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_DAY);
//            info->hour = array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_HOUR);
//            info->minute = array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_MINUTE);
//            info->second = array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_SECOND);
//            info->Inumber= array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_INUMBER);
//            info->Onumber= array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_ONUMBER);
//            info->money= array.get<jsonxx::Object>(i).
//                    get<jsonxx::Number>(JSON_KEY_MONEY);
//            if (NewTransaction(info->Onumber, info->Inumber, info->money,
//                               info->year, info->month, info->day,
//                               info->hour, info->minute, info->second) == 0) {
//                successnumber++;    //统计记录数目
//            }
//        }
//        return successnumber;
//    } else {
//        return false;
//    }
//    this->WriteInf("test.json");
}

string CCMIS::GenerateTag(int onum, int inum, int mon)
{
    time_t tt = time(NULL);
    tm* t= localtime(&tt);

    /**TIME*/
    string tag = to_string(t->tm_year + 1900);

    tag += t->tm_mon + 1 < 10 ? "0" + to_string(t->tm_mon + 1)
                              : to_string(t->tm_mon + 1);
    tag += t->tm_mday < 10 ? "0" + to_string(t->tm_mday)
                           : to_string(t->tm_mday);
    tag += t->tm_hour < 10 ? "0" + to_string(t->tm_hour)
                           : to_string(t->tm_hour);
    tag += t->tm_min < 10 ? "0" + to_string(t->tm_min)
                          : to_string(t->tm_min);
    tag += t->tm_sec < 10 ? "0" + to_string(t->tm_sec)
                          : to_string(t->tm_sec);

    /**I/O NUMBER*/
    tag += onum < 1000 ? "000" + to_string(onum) : to_string(onum);
    tag += inum < 1000 ? "000" + to_string(inum) : to_string(inum);

    /**COST*/
    if (mon < 10)
    {
        tag += "0000" + to_string(mon);
    } else if (mon < 100)
    {
        tag += "000" + to_string(mon);
    } else if (mon < 1000)
    {
        tag += "00" + to_string(mon);
    } else if (mon < 10000)
    {
        tag += "0" + to_string(mon);
    } else {
        tag += to_string(mon);
    }


    cout<<tag<<endl;

    return tag;
}

string CCMIS::GenerateTag(int year, int month, int day, int hour, int min, int sec, int onum, int inum, int mon)
{

    /**TIME*/
    string tag = to_string(year);

    tag += month < 10 ? "0" + to_string(month)
                              : to_string(month);
    tag += day < 10 ? "0" + to_string(day)
                           : to_string(day);
    tag += hour < 10 ? "0" + to_string(hour)
                           : to_string(hour);
    tag += min < 10 ? "0" + to_string(min)
                          : to_string(min);
    tag += sec < 10 ? "0" + to_string(sec)
                          : to_string(sec);

    /**I/O NUMBER*/
    tag += onum < 1000 ? "000" + to_string(onum) : to_string(onum);
    tag += inum < 1000 ? "000" + to_string(inum) : to_string(inum);

    /**COST*/
    if (mon < 10)
    {
        tag += "0000" + to_string(mon);
    } else if (mon < 100)
    {
        tag += "000" + to_string(mon);
    } else if (mon < 1000)
    {
        tag += "00" + to_string(mon);
    } else if (mon < 10000)
    {
        tag += "0" + to_string(mon);
    } else {
        tag += to_string(mon);
    }


    cout<<tag<<endl;

    return tag;
}

void CCMIS::InsertInf(Information* tempinf)
{
    Information* in = mInfo;

    while (in->next != NULL && Information::InfoToDateTime(in->next) < Information::InfoToDateTime(tempinf)) {
        in = in->next;
    }

    tempinf->next = in->next;
    in->next = tempinf;

    totalInfoCount++;
}

Information* CCMIS::BuildInfo(int onum, int inum, int mon)
{
    Information* info = new Information();
    info->tag = GenerateTag(onum, inum, mon);

    time_t tt = time(NULL);
    tm* t= localtime(&tt);

    info->year = t->tm_year + 1900;
    info->month = t->tm_mon + 1;
    info->day = t->tm_mday;
    info->hour = t->tm_hour;
    info->minute = t->tm_min;
    info->second = t->tm_sec;

    info->Onumber = onum;
    info->Inumber = inum;
    info->money = mon;

    info->next = NULL;

    return info;
}

Information* CCMIS::BuildInfo(int year, int month, int day, int hour, int min, int sec, int onum, int inum, int mon)
{
    Information* info = new Information();
    info->tag = GenerateTag(year, month, day, hour, min, sec, onum, inum, mon);

    info->year = year;
    info->month = month;
    info->day = day;
    info->hour = hour;
    info->minute = min;
    info->second = sec;

    info->Onumber = onum;
    info->Inumber = inum;
    info->money = mon;

    info->next = NULL;

    return info;
}

bool CCMIS::CheckPassword(string password)
{
    if (mUserNumber == -1) return false;    //用户名未初始化

    if (mUserNumber < 4000) //店家登录
    {
        //禁止登录洗漱券账户
        if (mUserNumber / 1000 == GROUP_BATH &&
                (mUserNumber / 10) % 10 >= 5) return false;

        Shop* s = mShop->next;
        while (s != NULL) {
            if (s->number == mUserNumber)
                break;
            s = s->next;
        }

        if (s !=NULL && password.compare(s->password) == 0)
            return true;
        else return false;
    } else {    //教职工登录
        User* u = mUser->next;
        while (u != NULL) {
            if (u->number == mUserNumber)
                break;
            u = u->next;
        }

        if (u !=NULL && password.compare(u->password) == 0)
            return true;
        else return false;
    }


}

void CCMIS::SetUserNumber(int n)
{
    mUserNumber = n;
}

Information* CCMIS::GetInfoPointer()
{
    return mInfo;
}

unsigned int CCMIS::GetTotalInfoNumber()
{
    return totalInfoCount;
}


unsigned int CCMIS::GetTotalUserNumber()
{
    return totalUserCount;
}


unsigned int CCMIS::GetTotalShopNumber()
{
    return totalShopCount;
}

//默认统计补贴信息 onum=2
Information* CCMIS::SearchInfoByOnum
(int onum , unsigned long start_date_num,unsigned int start_time_num ,
 unsigned long finish_date_num ,unsigned int finish_time_num )
{
    Information* filter_info_list = new Information();
    filter_info_list->next = NULL;

    Information* info = mInfo->next;

    unsigned long long start   = start_date_num *  1000000 + start_time_num;
    unsigned long long finish  = finish_date_num * 1000000 + finish_time_num;
    while(info!=NULL){

        if((int)info->Onumber == onum ){
            unsigned long  info_date = Date::DateToNum
                    (info->year,info->month,info->day);
            unsigned int info_time = Time::TimeToNum
                    (info->hour,info->minute);
            unsigned long long  info_cur = info_date * 1000000 + info_time;
            if( info_cur>=start && info_cur <=finish ){
                    Information* filtered_one_info = new Information();
                    Information::CopyOneInfo(info,filtered_one_info);
                    filtered_one_info->next = filter_info_list->next;
                    filter_info_list -> next = filtered_one_info;
            }
        }
        info = info->next;

    }
    return filter_info_list;
}

Information* CCMIS::SearchInfoByInum
(int inum, unsigned long start_date_num,unsigned int start_time_num,
 unsigned long finish_date_num,unsigned int finish_time_num)
{
    Information* filter_info_list = new Information();
    filter_info_list->next = NULL;

    Information* info = mInfo->next;

    unsigned long long start   = start_date_num *  1000000 + start_time_num;
    unsigned long long finish  = finish_date_num * 1000000 + finish_time_num;
    while(info!=NULL){

        if((int)info->Inumber == inum ){
            unsigned long  info_date = Date::DateToNum(info->year,info->month,info->day);
            unsigned int info_time = Time::TimeToNum(info->hour,info->minute);
            unsigned long long  info_cur = info_date * 1000000 + info_time;
            if( info_cur>=start && info_cur <=finish ){
                    Information* filtered_one_info = new Information();
                    Information::CopyOneInfo(info,filtered_one_info);
                    filtered_one_info->next = filter_info_list->next;
                    filter_info_list -> next = filtered_one_info;
            }
        }
        info = info->next;

    }
    return filter_info_list;

}

void CCMIS::DeleteInf(Information *tempinf)
{
    if (tempinf == NULL)
        return;

    Information* i = mInfo;
    while (i != NULL) {
        if (i->next == tempinf)
            break;
        i = i->next;
    }

    if (i != NULL)
    {
        qDebug()<<QString::fromLocal8Bit(i->tag.data());
        i->next = tempinf->next;
        delete tempinf;

        totalInfoCount--;
    }
}

User* CCMIS::GetUserByNum(int num)
{
    if (num >= USER_BEGIN && num <= USER_END)
    {
        User* u = mUser->next;
        while (u !=NULL) {
            if (u->number == num)
                return u;
            u = u->next;
        }

        return NULL;
    } else {
        return NULL;
    }
}

Shop* CCMIS::GetShopByNum(int num)
{
    if (
            (num >= SHOP_CANTEEN_BEGIN &&
            num <= SHOP_CANTEEN_END) ||
            (num >= SHOP_MARKET_BEGIN &&
             num <= SHOP_MARKET_END) ||
            (num >= SHOP_BATH_BEGIN &&
            num <= SHOP_BATH_END))   //防止越权取得0、1、2号用户权限
    {
        Shop* s = mShop->next;
        while (s != NULL) {
            if (s->number == num)
                return s;
            s = s->next;
        }

        return NULL;
    } else {
        return NULL;
    }
}

QString CCMIS::GetAllNameByNum(int num)
{
    switch (num) {
        case 0:
            return QString::fromUtf8("总入账账户");
            break;
        case 1:
            return QString::fromUtf8("充值账户");
        case 2:
            return QString::fromUtf8("返补助账户");
        default:
        {
            User* temp_user = GetUserByNum(num);
            Shop* temp_shop = GetShopByNum(num);
            if(temp_shop)
                return temp_shop->name;
            else if(temp_user)
                return temp_user->name;
            else
                return QString::fromUtf8("用户不存在");
            break;
        }
    }
}

QString CCMIS::GetCurrentUserName(){
    return this->GetAllNameByNum(mUserNumber);
}

User*   CCMIS::GetCurrentUser(){
    return GetUserByNum(mUserNumber);
}
Shop*   CCMIS::GetCurrentShop(){
    return GetShopByNum(mUserNumber);
}




int CCMIS::GetTotalCanteenConsumptionByDay(int year, int month, int day, int num)
{
    int c =0;
    Information* info = mInfo->next;
    while (info != NULL) {
        if (
                info->Inumber / 1000 == GROUP_CANTEEN &&
                info->year == (unsigned int)year &&
                info->month == (unsigned int)month &&
                info->day == (unsigned int)day &&
                info->Onumber == (unsigned int)num)
        {
            c += info->money;
        }

        info = info->next;
    }

    return c;
}

int CCMIS::GetTotalCanteenAndMarketConsumptionByDay
(int year, int month, int day, int num)
{
    int c =0;
    Information* info = mInfo->next;
    while (info != NULL) {
        if (
                (info->Inumber / 1000 == GROUP_CANTEEN ||
                 info->Inumber / 1000 == GROUP_MARKET) &&
                info->year == (unsigned int)year &&
                info->month == (unsigned int)month &&
                info->day == (unsigned int)day &&
                info->Onumber == (unsigned int)num)
        {
            c += info->money;
        }

        info = info->next;
    }

    return c;
}

int CCMIS::NewSubsidy(User* u)
{
    if (u->number >= USER_TEA_EMP_BEGIN &&
            u->number <= USER_TEA_EMP_END)   //教职工单次消费超20
    {
        u->balance += 500;
        //WriteUser(USER_FILE_NAME);

        Information* info = BuildInfo(2, u->number, 500);
        InsertInf(info);
        //WriteInf(INFO_FILE_NAME);

        JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
        jthread->start();
        jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
        jthread->start();
        return MESSAGE_TRANSACTION_SUCCESS;
    }else{
        return MESSAGE_TRANSACTION_UNKNOWN;
    }
}

int CCMIS::NewTransaction(int onum, int inum, int mon)
{
    if (mon <= 0)
        return MESSAGE_TRANSACTION_MONEY_LOWER_THAN_ZERO;

    User* u = GetUserByNum(onum);
    if (u == NULL)
        return MESSAGE_TRANSACTION_NO_USER;     //学生、教职工号不存在，交易失败

    Shop* s = GetShopByNum(inum);
    if (s == NULL)
        return MESSAGE_TRANSACTION_NO_SHOP;     //商号不存在，交易失败

    time_t tt = time(NULL);
    tm* t= localtime(&tt);


    if (inum / 1000 == GROUP_CANTEEN)           //收款方是食堂组
    {
        if (mon > 5000 || GetTotalCanteenConsumptionByDay
                (t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, onum) + mon > 10000)
        {
            return MESSAGE_TRANSACTION_OVERFLOW;    //超过每笔消费或单日限制
        } else {    //交易条件具备，开始交易

            if (u->balance < mon)  //余额不足
                return MESSAGE_TRANSACTION_BALANCE_NOT_ENOUGH;

            u->balance -= mon;
            //WriteUser(USER_FILE_NAME);
            Information* info = BuildInfo(onum, inum, mon);
            InsertInf(info);
            //WriteInf(INFO_FILE_NAME);

            if (u->number >= USER_TEA_EMP_BEGIN &&
                    u->number <= USER_TEA_EMP_END &&
                    mon > 2000)   //教职工单次消费超20
            {
                NewSubsidy(u);
            } else {
                JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
                jthread->start();
                jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
                jthread->start();
            }
            return MESSAGE_TRANSACTION_SUCCESS;
        }
    } else if (inum / 1000 == GROUP_MARKET) {   //收款方是超市组
        if (GetTotalCanteenAndMarketConsumptionByDay
                (t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, onum) + mon > 10000)
        {
            return MESSAGE_TRANSACTION_OVERFLOW;    //超过单日限制
        } else {
            if (u->balance < mon)  //余额不足
                return MESSAGE_TRANSACTION_BALANCE_NOT_ENOUGH;

            u->balance -= mon;
            //WriteUser(USER_FILE_NAME);
            Information* info = BuildInfo(onum, inum, mon);
            InsertInf(info);
            //WriteInf(INFO_FILE_NAME);

            JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
            jthread->start();
            jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
            jthread->start();

            return MESSAGE_TRANSACTION_SUCCESS;
        }
    } else if (inum / 1000 == GROUP_BATH) {
        if (u->balance + u->coupon < mon)   //余额不足
            return MESSAGE_TRANSACTION_BALANCE_NOT_ENOUGH;

        if (mon < 200)
            mon = 200;    //至少消费2元

        if (u->coupon > 0)   //洗漱券有余额
        {
            if (u->coupon >= mon)   //洗漱券余额超过 mon
            {
                u->coupon -= mon;

                Information* info = BuildInfo(onum, inum + 50, mon);    //转券账户
                InsertInf(info);
            } else {    //否则先扣coupon，再扣balance
                u->balance -= (mon - u->coupon);

                if (u->coupon != 0)
                {
                    //转券账户
                    Information* info = BuildInfo(onum, inum + 50, u->coupon);
                    InsertInf(info);
                }

                //转普通账户
                Information* info = BuildInfo(onum, inum, mon - u->coupon);
                InsertInf(info);

                u->coupon = 0;
            }
        } else {
            u->balance -= mon;
        }

        //WriteUser(USER_FILE_NAME);

        //WriteInf(INFO_FILE_NAME);

        JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
        jthread->start();
        jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
        jthread->start();

        return MESSAGE_TRANSACTION_SUCCESS;
    }

    return MESSAGE_TRANSACTION_UNKNOWN; //未知错误
}

int CCMIS::NewAdmTransaction(int year, int month, int day, int hour, int min, int sec, int onum, int inum, int mon)
{
    if (mon <= 0)
        return MESSAGE_TRANSACTION_MONEY_LOWER_THAN_ZERO;

    User* u = GetUserByNum(onum);
    if (u == NULL)
        return MESSAGE_TRANSACTION_NO_USER;     //学生、教职工号不存在，交易失败

    Shop* s = GetShopByNum(inum);
    if (s == NULL)
        return MESSAGE_TRANSACTION_NO_SHOP;     //商号不存在，交易失败

    if (inum / 1000 == GROUP_CANTEEN)           //收款方是食堂组
    {
        //无单日限制

        if (u->balance < mon)  //余额不足
            return MESSAGE_TRANSACTION_BALANCE_NOT_ENOUGH;

        u->balance -= mon;
        //WriteUser(USER_FILE_NAME);
        Information* info = BuildInfo(year, month, day, hour, min, sec, onum, inum, mon);
        InsertInf(info);
        //WriteInf(INFO_FILE_NAME);

        if (u->number >= USER_TEA_EMP_BEGIN &&
                u->number <= USER_TEA_EMP_END &&
                mon > 2000)   //教职工单次消费超20
        {
            NewSubsidy(u);
        } else {
            JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
            jthread->start();
            jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
            jthread->start();
        }
        return MESSAGE_TRANSACTION_SUCCESS;

    } else if (inum / 1000 == GROUP_MARKET) {   //收款方是超市组

        if (u->balance < mon)  //余额不足
            return MESSAGE_TRANSACTION_BALANCE_NOT_ENOUGH;

        u->balance -= mon;
        //WriteUser(USER_FILE_NAME);
        Information* info = BuildInfo(year, month, day, hour, min, sec, onum, inum, mon);
        InsertInf(info);
        //WriteInf(INFO_FILE_NAME);

        JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
        jthread->start();
        jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
        jthread->start();

        return MESSAGE_TRANSACTION_SUCCESS;

    } else if (inum / 1000 == GROUP_BATH) {
        if (u->balance + u->coupon < mon)   //余额不足
            return MESSAGE_TRANSACTION_BALANCE_NOT_ENOUGH;

        if (mon < 200)
            mon = 200;    //至少消费2元

        if (u->coupon > 0)   //洗漱券有余额
        {
            if (u->coupon >= mon)   //洗漱券余额超过 mon
            {
                u->coupon -= mon;

                Information* info = BuildInfo(year, month, day, hour, min, sec, onum, inum + 50, mon);    //转券账户
                InsertInf(info);
            } else {    //否则先扣coupon，再扣balance
                u->balance -= (mon - u->coupon);

                if (u->coupon != 0)
                {
                    //转券账户
                    Information* info = BuildInfo(year, month, day, hour, min, sec, onum, inum + 50, u->coupon);
                    InsertInf(info);
                }

                //转普通账户
                Information* info = BuildInfo(year, month, day, hour, min, sec, onum, inum, mon - u->coupon);
                InsertInf(info);

                u->coupon = 0;
            }
        } else {
            u->balance -= mon;
        }

        //WriteUser(USER_FILE_NAME);

        //WriteInf(INFO_FILE_NAME);

        JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
        jthread->start();
        jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
        jthread->start();

        return MESSAGE_TRANSACTION_SUCCESS;
    }

    return MESSAGE_TRANSACTION_UNKNOWN; //未知错误
}

bool CCMIS::NewRefund(Information *tempinf)
{
    qDebug()<<tempinf;
    if (tempinf == NULL)
        return false;

    User* u = mUser->next;
    while (u != NULL) {
        if (u->number == tempinf->Onumber)
            break;
        u = u->next;
    }

    if (u == NULL)
        return false;

    if (    //券消费
            tempinf->Inumber / 1000 == GROUP_BATH &&
            (tempinf->Inumber / 10) % 10 >= 5)
    {
        u->coupon += tempinf->money;
    } else {
        u->balance += tempinf->money;
    }

    DeleteInf(tempinf);

    //WriteUser(USER_FILE_NAME);
    //WriteInf(INFO_FILE_NAME);

    JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
    jthread->start();
    jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
    jthread->start();

    return true;
}

int CCMIS::GetUserNum()
{
    return mUserNumber;
}





bool CCMIS::NewRecharge(int num, int money)
{
    User* u = GetUserByNum(num);
    u->balance += money;
    Information* info = BuildInfo(1, num, money);
    InsertInf(info);

    JsonThread* jthread = new JsonThread(this, THREAD_TYPE_W_USER);
    jthread->start();
    jthread = new JsonThread(this, THREAD_TYPE_W_INFO);
    jthread->start();

    //return WriteUser(USER_FILE_NAME) && WriteInf(INFO_FILE_NAME);

    return true;
}

Information* CCMIS::GetInfoByTag(QString tag)
{
    Information* i = mInfo->next;
    while (i != NULL) {
        if (i->tag.compare(COMMON_FUNCS::ToUTF8String(tag)) == 0){
            return i;
        }
        i = i->next;
    }
    return NULL;
}

void CCMIS::CouponFresh()
{
//    User* u = mUser->next;
//    QDateTime *datetime = QDateTime::currentDateTime();

//    if (datetime->date()->day() == 1
//        && datetime->time()->hour() == 0
//        && datetime->time()->minute() == 0
//        && datetime->time()->second() == 0){
//        while (u !=NULL) {
//            if (num >= USER_BEGIN && num < USER_TEA_EMP_BEGIN){
//                u->coupon = 10000;
//            }
//            u = u->next;
//        }
//        WriteUser(USER_FILE_NAME);
//    }
}
