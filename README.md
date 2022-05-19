# lua-chunk 从zip压缩包加载文件
### 1.依赖 libzip , https://libzip.org/
### 2.在skynet内测试代码
    ```
    local skynet = require "skynet"
    skynet.sleep(100)
    local chunk = require("chunk")
    local success = chunk.loadzip("testzip.zip","222222")

    skynet.newservice("testservice")
    Log.i("success ==",success)
    skynet.sleep(200)
    os.exit()
    ```

skynet.newservice("testservice") 加载有2种方式
1. 修改loader，看本例子
2. 修改lua源码，新增如下函数 lauxlib.h
   #define luaL_loadbuffernew(L,s,sz,n)	luaL_loadbufferxex(L,s,sz,n,NULL)
   LUALIB_API int (luaL_loadbufferxex) (lua_State *L, const char *buff, size_t sz,const char *name, const char *mode);

   <!-- 测试代码，丑了一些 -->
   LUALIB_API int luaL_loadbufferxex (lua_State *L, const char *buff, size_t size,
                                 const char *name, const char *mode) {
  LoadS ls;
  ls.s = buff;
  ls.size = size;
   printf("00000 ==%s,%d,name=%s\n",buff,size,name);
  lua_State * eL = luaL_newstate();
  if (eL == NULL) {
    lua_pushliteral(L, "New state failed");
    return LUA_ERRMEM;
  }
  <!-- 只是为了构造搜索路径，根据自身需要修改 -->
  char filename[128] = {0};
  int tmpsize = strlen(name)-2-strlen(".lua");
  memcpy(filename,name+2,tmpsize);
  unsigned short filenamelen = strlen(filename);
  int j;
  for(j=0;j<filenamelen;j++)
  {
      if(filename[j] == '/')
      {
          filename[j] = '.';
      }
  }
  printf("11111122 ===name=%s size=%d\n",filename,tmpsize);
   printf("gettop1 ==%d\n",lua_gettop(eL));
  int status = lua_load(eL, getS, &ls, name, mode);
   printf("gettop2 ==%d\n",lua_gettop(eL));
  printf("11111222 ===%d,ep =%p\n",status,eL);

  printf("2222\n");
  const void * proto = lua_topointer(eL, -1);
   printf("gettop3 ==%d\n",lua_gettop(eL));
  printf("33\n");

  const void * oldv = save(name, proto);
  if (oldv) {
    lua_close(eL);
    lua_clonefunction(L, oldv);
  } else {
    lua_clonefunction(L, proto);
    /* Never close it. notice: memory leak */
  }

  return LUA_OK;
}
