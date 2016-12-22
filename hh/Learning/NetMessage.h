#pragma once
enum NetMessage
{
	GM_CREATE_ACCOUNT,			//创建账号
	GM_CREATE_ACCOUNT_RESPONSE,

	GM_ACCOUNT_CHECK,
	GM_ACCOUNT_CHECK_RESPONSE,

	GM_CREATE_PLAYER,
	GM_CREATE_PLAYER_RESPONSE,	

	GM_LOGIN,
	GM_LOGIN_RESPONSE,

	PLAYERMESSAGEBEGIN = 100000,

	GM_HEARTBEAT_FROM_CLIENT = 100050,						/**< 与客户端心跳 */
	GM_CLIENT_HEARTBEAT_RESPONSE,							/**< 对客户端心跳包的响应 */

	//-------------------------------------------------------------------------------------
	GM_REQUEST_XINFA = 101000,										/**< 请求心法信息 */
	GM_REQUEST_XINFA_RESPONSE,
	GM_REQUEST_DAZUO,												/**< 请求打坐 */
	GM_REQUEST_DAZUO_RESPONSE,
	GM_REQUEST_TIAOXI,												/**< 请求调息 */
	GM_REQUEST_TIAOXI_RESPONSE,

	//-------------------------------------------------------------------------------------
	GM_REQUEST_MOVE = 102000,				//请求玩家移动
	GM_NOTIFY_MOVEING,						//广播玩家移动
	GM_REQUEST_USE,							//请求使用道具
	GM_USE_RESPONSE,	
	PLAYERMESSAGEEND,
};