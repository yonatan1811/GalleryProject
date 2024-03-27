#include "DataBaseAccess.h"
#include <io.h>
using namespace std;
static int tagid = 0;
static int albumId = 0;

//albums:::


int callBackPictures(void* data, int agrc, char** argv, char** azColName)
{
	std::list<Picture>* l = (std::list<Picture>*)(data);
	l->push_back(Picture(std::stoi(argv[0]), argv[1], argv[2], argv[3]));
	return 0;

}

int callBackTags(void* data, int agrc, char** argv, char** azColName)
{
	std::list<int>* l = (std::list<int>*)(data);
	l->push_back(std::stoi(argv[2])); //user_id
	return 0;
}

int getLastTagId(void* data, int agrc, char** argv, char** azColName)
{
	int id = std::stoi(argv[0]);
	return 0;
}

int callBackCountAlbum(void* data, int agrc, char** argv, char** azColName)
{
	int* id = (int*)(data);
	*id = std::stoi(argv[0]);
	return 0;
}
int callBackAlbumId(void* data, int agrc, char** argv, char** azColName)
{
	int* id = (int*)(data);
	*id = std::stoi(argv[0]);
	return 0;
}

int callBackUserId(void* data, int argc, char** argv, char** azColName)
{
	int* id = (int*)(data);
	*id = std::stoi(argv[0]);
	return 0;
}

int callBackTagStats(void* data, int argc, char** argv, char** azColName)
{
	int* count = (int*)(data);
	*count = std::stoi(argv[0]);
	return 0;
}
int callBackAvg(void* data, int argc, char** argv, char** azColName)
{
	float* avg = (float*)data;
	*avg = std::atof(argv[0]);
	return 0;
}



int callBackAlbum(void* data, int agrc, char** argv, char** azColName)
{
	std::list<Album>* listOfAlbums = (std::list<Album>*)(data);
	listOfAlbums->push_back(Album(std::stoi(argv[3]), argv[1], argv[2]));
	return 0;
}

int callBackUsers(void* data, int agrc, char** agrv, char** azColName)
{
	std::list<User>* listOfUsers = (std::list<User>*)(data);
	listOfUsers->push_back(User(std::stoi(agrv[0]), agrv[1]));
	return 0;
}

void DataBaseAccess::createAlbum(const Album& album) 
{
	std::string sqlState = "INSERT INTO ALBUMS(ID , NAME , CREATION_DATE , USER_ID) VALUES(" + std::to_string(albumId) + ", \"" + album.getName() + "\", \"" + album.getCreationDate() + "\"," + std::to_string(album.getOwnerId()) + ");";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt create album" << std::endl;
	}
}


const std::list<Album> DataBaseAccess::getAlbumsOfUser(const User& user)
{
	std::list<Album>* list = new std::list<Album>();
	std::string sql = "SELECT * FROM ALBUMS WHERE USER_ID = " + std::to_string(user.getId()) + ";";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sql.c_str(), callBackAlbum, list, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete album" << std::endl;
	}

	std::list<Picture>* pic = new std::list<Picture>();
	int id = 0;
	res = sqlite3_exec(db, sql.c_str(), callBackAlbumId, &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt open album" << std::endl;
	}

	//getting all the pictures of the album
	sql = "";
	sql = "SELECT * FROM PICTURES WHERE ALBUM_ID LIKE " + std::to_string(id) + ";";
	res = sqlite3_exec(db, sql.c_str(), callBackPictures, pic, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt open album" << std::endl;
	}

	for (auto i = pic->begin(); i != pic->end(); i++)
	{
		std::list<int>* tags = new std::list<int>();
		sql = "SELECT * FROM TAGS WHERE PICTURE_ID = \"" + i->getName() + "\";";
		res = sqlite3_exec(db, sql.c_str(), callBackTags, tags, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "not Working didnt get tags" << std::endl;
			throw exception();
		}
		for (auto j = tags->begin(); j != tags->end(); j++)
		{
			i->tagUser(*j);
		}
	}
	//adding the pictures
	for (auto i = pic->begin(); i != pic->end(); i++)
	{
		(list->begin())->addPicture(*i);
	}


	return *list;
}


bool DataBaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	std::string sqlState = "SELECT COUNT(1) FROM albums WHERE user_id = " + std::to_string(userId) + " AND name = '" + albumName + "';";
	char* errMessage = nullptr;
	int id = 0;
	int res = sqlite3_exec(db, sqlState.c_str(), callBackCountAlbum , &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt find album" << std::endl;
	}
	if (id != 0)
	{
		return true;
	}
	return false;
}

Album DataBaseAccess::openAlbum(const std::string& albumName)
{
	string sql = "SELECT * FROM ALBUMS WHERE NAME LIKE \"" + albumName + "\";";
	std::list<Album>* l = new std::list<Album>();
	
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sql.c_str(), callBackAlbum, l, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt open album" << std::endl;
	}



	std::list<Picture>* pic = new std::list<Picture>();
	int id = 0;
	res = sqlite3_exec(db, sql.c_str(), callBackAlbumId, &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt open album" << std::endl;
	}

	//getting all the pictures of the album
	sql = "";
	sql = "SELECT * FROM PICTURES WHERE ALBUM_ID LIKE " + std::to_string(id) + ";";
	res = sqlite3_exec(db, sql.c_str(), callBackPictures, pic, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt open album" << std::endl;
	}

	for (auto i = pic->begin(); i != pic->end(); i++)
	{
		std::list<int>* tags = new std::list<int>();
		sql = "SELECT * FROM TAGS WHERE PICTURE_ID = \"" + i->getName() + "\";";
		res = sqlite3_exec(db, sql.c_str(), callBackTags, tags, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "not Working didnt get tags" << std::endl;
			throw exception();
		}
		for (auto j = tags->begin(); j != tags->end(); j++)
		{
			i->tagUser(*j);
		}
	}
	//adding the pictures
	for (auto i = pic->begin(); i != pic->end(); i++)
	{
		(l->begin())->addPicture(*i);
	}
	
	return *l->begin();


}

void DataBaseAccess::printAlbums()
{
	std::list<Album>* list = new std::list<Album>();
	std::string sqlState = "SELECT * FROM ALBUMS;";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), callBackAlbum, list, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete album" << std::endl;
	}
	int id = 0;
	for (auto i = list->begin(); i != list->end(); i++)
	{
		std::cout << albumId << "|" + i->getName() + "|" << i->getCreationDate() + "|" << i->getOwnerId() << std::endl;
		albumId++;
	}
}


bool DataBaseAccess::open()
{

	sqlite3* db;
	string dbFileName = "MyDB1.1.sqlite"; // creating a new db 
	int file_exist = _access(dbFileName.c_str(), 0);
	int res = sqlite3_open(dbFileName.c_str(), &db);
	if (res != SQLITE_OK) {
		db = nullptr;
		cout << "Failed to open DB" << endl;
		return false;
	}
	this->db = db;

	char* errMessage = nullptr;
	//reseting the tagid's
	string sql = "SELECT * FROM TAGS ORDER BY ID DESC;";
	int last = 0;
	res = sqlite3_exec(db, sql.c_str(), getLastTagId, &last, &errMessage);
	tagid = last+1;



	const char* sqlStatement = "CREATE TABLE IF NOT EXISTS USERS (ID INT PRIMARY KEY NOT NULL, NAME TEXT NOT NULL);";
	errMessage = nullptr;
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		db = nullptr;
		return false;	
	}
	sqlStatement = "CREATE TABLE IF NOT EXISTS ALBUMS( ID INT PRIMARY KEY NOT NULL , NAME TEXT , CREATION_DATE TEXT , USER_ID INT , FOREIGN KEY(USER_ID) REFERENCES USERS(ID));";
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		db = nullptr;
		return false;
	}
	sqlStatement = "CREATE TABLE IF NOT EXISTS PICTURES(ID INT  PRIMARY KEY , NAME TEXT , LOCATION TEXT , CREATION_DATE TEXT , ALBUM_ID INT , FOREIGN KEY(ALBUM_ID) REFERENCES ALBUMS(ID));";
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		db = nullptr;
		return false;
	}
	sqlStatement = "CREATE TABLE IF NOT EXISTS TAGS (ID INT PRIMARY KEY , PICTURE_ID INT , USER_ID INT , FOREIGN KEY (USER_ID) REFERENCES USERS(ID));";
	res = sqlite3_exec(db, sqlStatement, nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		db = nullptr;
		return false;
	}


	return true;
}

void DataBaseAccess::close()
{
	sqlite3_close(db);
	db = nullptr;
}

void DataBaseAccess::clear()
{
	

}

//picture ::

int callbackPicture(void* data, int argc, char** argv, char** azColName)
{
	Picture* pic = (Picture*)data;
	pic->setId(std::stoi(argv[0]));
	pic->setName(argv[1]);
	pic->setPath(argv[2]);
	pic->setCreationDate(argv[3]);
	return 0;
}

void DataBaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	std::string sqlState = "SELECT * FROM ALBUMS WHERE NAME = \"" + albumName + "\";";
	int id = 0;
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), callBackAlbumId, &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt add pic" << std::endl;
		
	}
	sqlState = "INSERT OR IGNORE INTO PICTURES VALUES(" + std::to_string(picture.getId()) + ", \"" + picture.getName() + "\", \"" + picture.getPath() + "\" , \"" + picture.getCreationDate() + "\" , " + std::to_string(id) + ");";
	res = sqlite3_exec(db, sqlState.c_str(), callBackAlbumId, &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt insert pic" << std::endl;
	}
	
	
}

void DataBaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	std::string sqlState = "SELECT * FROM ALBUMS WHERE NAME = \"" + albumName + "\";";
	int id = 0;
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), callBackAlbumId, &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete pic" << std::endl;
		
	}
	sqlState = "DELETE FROM PICTURES WHERE NAME = \" " + pictureName + "\" AND ALBUM_ID = " + std::to_string(id) + ";";
	res = sqlite3_exec(db, sqlState.c_str(), callBackAlbumId, &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete pic" << std::endl;

	}
}




//albums::


const std::list<Album> DataBaseAccess::getAlbums()
{
	std::list<Album>* list = new std::list<Album>();
	std::string sqlState = "SELECT * FROM ALBUMS;";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), callBackAlbum, list, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete album" << std::endl;
	}
	return *list;
}

void DataBaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::string sqlState = "DELETE FROM ALBUMS WHERE USER_ID LIKE " + std::to_string(userId) + ";";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete album" << std::endl;
	}


}

void DataBaseAccess::closeAlbum(Album& pAlbum)
{
}


//users::
void DataBaseAccess::createUser(User& user)
{
	int id = user.getId();
	std::string name = user.getName();
	std::string sqlstate = "INSERT INTO USERS (ID , NAME) VALUES (" + std::to_string(id) + "," + + "\"" + (name)+ "\"" + ");";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlstate.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt add user" << std::endl;
	}

}

void DataBaseAccess::deleteUser(const User& user)
{
	int id = user.getId();
	std::string name = user.getName();

	std::string sqlState = "DELETE FROM USERS WHERE ID  = " + std::to_string(id) + ";";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt add user" << std::endl;
	}
}

void DataBaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	std::string fixName = "\"" + pictureName + "\"";
	std::string sqlState = "SELECT * FROM PICTURES WHERE NAME = " + fixName + ";";
	Picture* pic = new Picture(0 , "null");
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), callbackPicture, pic, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "didnt manage to tag in picture named:" << pictureName << std::endl;
		return;
	}



	sqlState = "INSERT INTO TAGS VALUES(" + std::to_string(tagid) + "," + "\""+(pic->getName())+"\"" + "," +"\"" + std::to_string(userId) +"\"" + ");";
	res = sqlite3_exec(db, sqlState.c_str(), callbackPicture, &pic, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "didnt manage to tag in picture named : " << pictureName << std::endl;
		return;
	}

	tagid++;

}

void DataBaseAccess::untagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{

	std::string fixName = "\"" + pictureName + "\"";
	std::string sqlState = "SELECT * FROM PICTURES WHERE NAME = " + fixName + ";";
	Picture* pic = new Picture(-1 , "NULL");
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), callbackPicture, pic, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "didnt manage to tag in picture named:" << pictureName << std::endl;
		throw exception();
	}

	sqlState = "DELETE FROM TAGS WHERE USER_ID = " + std::to_string(userId) + ";";
	res = sqlite3_exec(db, sqlState.c_str(), callbackPicture, &pic, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "didnt manage to tag in picture named : " << pictureName << std::endl;
		exception();
	}
}

void DataBaseAccess::printUsers()
{
	std::list<User>* list = new std::list<User>();
	std::string sqlState = "SELECT * FROM USERS;";
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlState.c_str(), callBackUsers, list, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete album" << std::endl;
	}
	for (auto i = list->begin(); i != list->end(); i++)
	{
		std::cout << i->getId() << "|" << i->getName() << std::endl;
	}

}

User DataBaseAccess::getUser(int userId)
{
	if (doesUserExists(userId) == true)
	{
		std::list<User>* list = new std::list<User>();

		std::string sqlState = "SELECT * FROM USERS WHERE ID =" + std::to_string(userId) + ";";
		char* errMessage = nullptr;
		int res = sqlite3_exec(db, sqlState.c_str(), callBackUsers, list, &errMessage);
		if (res != SQLITE_OK)
		{
			std::cout << "not working , didnt delete album" << std::endl;
			return User(-1, "not working");
		}
		return *list->begin();
	}
	return User(-1, "not working");

}


bool DataBaseAccess::doesUserExists(int userId)
{
	std::string sqlState = "SELECT COUNT(1) FROM USERS WHERE ID = " + std::to_string(userId) + ";";
	char* errMessage = nullptr;
	int id = 0;
	int res = sqlite3_exec(db, sqlState.c_str(), callBackUserId, &id, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "not working , didnt delete album" << std::endl;
	}
	if (id != 0)
	{
		return true;
	}
	return false;

}

int DataBaseAccess::countAlbumsOwnedOfUser(const User& user)
{

	std::string sqlstate = "SELECT COUNT (ID) FROM ALBUMS  WHERE USER_ID  = " + std::to_string(user.getId()) + ";";
	int count = 0;
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlstate.c_str(), callBackTagStats, &count, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "didnt find user" << std::endl;
		return -1;
	}

	return count;
}

int DataBaseAccess::countAlbumsTaggedOfUser(const User& user)
{

	std::string sqlstate = "SELECT COUNT (USER_ID) FROM TAGS INNER JOIN USERS ON TAGS.USER_ID = USERS.ID WHERE USER_ID = " + std::to_string(user.getId()) + ";"; //idan helped me with this one , not lying
	int count = 0;
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlstate.c_str(), callBackTagStats, &count, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "didnt find user" << std::endl;
		return -1;
	}
	return count;
}


int DataBaseAccess::countTagsOfUser(const User& user)
{
	std::string sqlstate = "SELECT COUNT (ID) FROM TAGS  WHERE USER_ID  = " + std::to_string(user.getId()) + ";";
	int count = 0;
	char* errMessage = nullptr;


	int res = sqlite3_exec(db, sqlstate.c_str(), callBackTagStats, &count, &errMessage);
	if (res != SQLITE_OK)
	{
		std::cout << "didnt find tag or user" << std::endl;
		return -1;
	}
	return count;
}



float DataBaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	int tags = countTagsOfUser(user);
	int albums = countAlbumsTaggedOfUser(user);
	if (0 == albums)
	{
		return 0;
	}
	return (float)tags / albums;
}


User DataBaseAccess::getTopTaggedUser()
{
	std::string sqlStat = "SELECT * FROM USERS WHERE ID = (SELECT USER_ID FROM TAGS GROUP BY USER_ID ORDER BY COUNT(*) DESC LIMIT 1);";
	std::list<User>* list = new std::list<User>();
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStat.c_str(), callBackUsers, list, &errMessage);
	User u = User(-1, "sw");
	if (res != SQLITE_OK)
	{
		std::cout << "didnt find album or user" << std::endl;
		return u;
	}
	if (list->size() == 0)
	{
		return u;
	}
	return *list->begin();
}

Picture DataBaseAccess::getTopTaggedPicture()
{
	std::string sqlStat = "SELECT * FROM PICTURES WHERE ID == (SELECT PICTURE_ID FROM TAGS GROUP BY PICTURE_ID ORDER BY COUNT(*) DESC LIMIT 1);";
	std::list<Picture>* list = new std::list<Picture>();
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStat.c_str(), callBackUsers, list, &errMessage);
	Picture u = Picture(-1, "sw");
	if (res != SQLITE_OK || list->size() == 0)
	{
		std::cout << "didnt find album or user" << std::endl;
		return u;
	}
	return *list->begin();

}

std::list<Picture> DataBaseAccess::getTaggedPicturesOfUser(const User& user)
{
	std::string sqlStat = "SELECT * FROM PICTURES INNER JOIN TAGS ON pictures.id = tags.picture_id where tags.user_id == " + std::to_string(user.getId()) + ";";
	std::list<Picture>* pics = new std::list<Picture>();
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, sqlStat.c_str(), callBackPictures, pics, &errMessage);
	Picture u = Picture(-1, "sw");
	if (res != SQLITE_OK)
	{
		std::cout << "didnt find album or user" << std::endl;
	}

	return *pics;


}