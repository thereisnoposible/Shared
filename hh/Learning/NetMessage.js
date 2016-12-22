module.exports = {
	"GM_CREATE_ACCOUNT":0,			//创建账号
	"GM_CREATE_ACCOUNT_RESPONSE":1,
	"GM_ACCOUNT_CHECK":2,
	"GM_ACCOUNT_CHECK_RESPONSE":3,
	"GM_CREATE_PLAYER":4,
	"GM_CREATE_PLAYER_RESPONSE":5,	
	"GM_LOGIN":6,
	"GM_LOGIN_RESPONSE":7,
	"PLAYERMESSAGEBEGIN ": 100000,
	"GM_HEARTBEAT_FROM_CLIENT ": 100050,						/**< 与客户端心跳 */
	"GM_CLIENT_HEARTBEAT_RESPONSE":100051,							/**< 对客户端心跳包的响应 */
	//-------------------------------------------------------------------------------------
	"GM_REQUEST_XINFA ": 101000,										/**< 请求心法信息 */
	"GM_REQUEST_XINFA_RESPONSE":101001,
	"GM_REQUEST_DAZUO":101002,												/**< 请求打坐 */
	"GM_REQUEST_DAZUO_RESPONSE":101003,
	"GM_REQUEST_TIAOXI":101004,												/**< 请求调息 */
	"GM_REQUEST_TIAOXI_RESPONSE":101005,
	//-------------------------------------------------------------------------------------
	"GM_REQUEST_MOVE ": 102000,				//请求玩家移动
	"GM_NOTIFY_MOVEING":102001,						//广播玩家移动
	"GM_REQUEST_USE":102002,							//请求使用道具
	"GM_USE_RESPONSE":102003,	
	"PLAYERMESSAGEEND":102004,
};
