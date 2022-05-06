
#include "lua.h"
#include "lauxlib.h"
#include <stdio.h>
/* For "exit". */
#include <stdlib.h>
/* For "strerror". */
#include <string.h>
/* For "errno". */
#include <errno.h>
#include <zip.h>

const unsigned short FILENAMELEN = 128;

static int
_lua_f_loadzip(lua_State *L)
{
    size_t filelen,pwdlen;
    const char* password = NULL;
    int argc = lua_gettop(L);
    luaL_checktype(L, 1, LUA_TSTRING);
    if(argc >= 2) {
        luaL_checktype(L, 2, LUA_TSTRING);
        password = lua_tolstring(L,2,&pwdlen);
    }
    
    // printf("1 top size ==%d\n",lua_gettop(L));
    const char* zipfilename = lua_tolstring(L, 1, &filelen);
    
    // printf("2 top size ==%d,zipfilename=%s\n",lua_gettop(L),zipfilename);
    
    // lua_settop(L, 0);
    // printf("22 top size ==%d\n",lua_gettop(L));

    int err = 0;
    struct zip *z = zip_open(zipfilename, 0, &err);

    if(z) {
        if(password) {
            //输入zip密码
            // printf("load top =%d,zip password=%s\n",lua_gettop(L),password);
            zip_set_default_password(z, password);
        }
        int fileCount,i,j;
        fileCount = zip_get_num_files(z);
        // printf("fileCount = %d\n",fileCount);
        char filename[FILENAMELEN] = {0};
        if(fileCount > 0) {
            // printf("3 top size ==%d\n",lua_gettop(L));
            lua_getglobal(L, "package");
            lua_getfield(L, -1, "preload");

            // printf("4 top size ==%d\n",lua_gettop(L));
            for (i = 0; i < fileCount ; i++) {
                struct zip_stat zipStat = {0};
                zip_stat_init(&zipStat);
                zip_stat_index(z, i, 0, &zipStat);
                char * hasdir = strchr(zipStat.name,'/');
                // 排除非法文件
                if ( (hasdir && hasdir[strlen(hasdir) - 1] == '/')  || strstr(zipStat.name,"/.") ) {
                    // printf("%dth discard name dir [%s]\n",i,zipStat.name);
                    continue;
                }
                // printf("%dth file name is [%s]\n",i,zipStat.name);
                // 查找到.lua文件
                char * dotlua = strstr(zipStat.name,".lua");
                memcpy(filename,zipStat.name, dotlua - zipStat.name);
                // 查找到有目录的情况,需要替换成.
                unsigned short filenamelen = strlen(filename);
                for(j=0;j<filenamelen;j++)
                {
                    if(filename[j] == '/')
                    {
                        filename[j] = '.';
                    }
                }
                // printf("%dth real filename is [%s]\n",i,filename);
                struct zip_file *pzipFile = zip_fopen_index(z, i, 0);
                char *buf = (char *) malloc(zipStat.size);
                zip_fread(pzipFile, buf, zipStat.size);

                err = luaL_loadbuffer(L, buf, zipStat.size, filename);
                if(err == LUA_OK) {
                    lua_setfield(L, -2, filename);
                    free(buf);
                    // 重置文件名
                    memset(filename,0,FILENAMELEN);
                }
                else
                {
                    break;
                }
            }
            //And close the archive
            zip_close(z);

            // printf("5 top size ==%d\n",lua_gettop(L));
            lua_pop(L, 2);
            // printf("6 top size ==%d\n",lua_gettop(L));
        }
    }

    // printf("7 top size ==%d\n",lua_gettop(L));

    lua_pushboolean(L,err == 0);
    // printf("8 top size ==%d\n",lua_gettop(L));
    return 1;
}

int luaopen_chunk(lua_State *L) {
    luaL_checkversion(L);

    luaL_Reg l[] = {
        {"loadzip", _lua_f_loadzip},
        {NULL, NULL}
    };
    luaL_newlib(L, l);
    return 1;
}

