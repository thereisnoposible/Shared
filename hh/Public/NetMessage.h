#pragma once
enum NetMessage
{
	GM_CREATE_ACCOUNT,			//�����˺�
	GM_CREATE_ACCOUNT_RESPONSE,

	GM_ACCOUNT_CHECK,
	GM_ACCOUNT_CHECK_RESPONSE,

	GM_CREATE_PLAYER,
	GM_CREATE_PLAYER_RESPONSE,	

	GM_LOGIN,
	GM_LOGIN_RESPONSE,

	PLAYERMESSAGEBEGIN = 100000,

	GM_HEARTBEAT_FROM_CLIENT = 100050,						/**< ��ͻ������� */
	GM_CLIENT_HEARTBEAT_RESPONSE,							/**< �Կͻ�������������Ӧ */

	//-------------------------------------------------------------------------------------
	GM_REQUEST_XINFA = 101000,										/**< �����ķ���Ϣ */
	GM_REQUEST_XINFA_RESPONSE,
	GM_REQUEST_DAZUO,												/**< ������� */
	GM_REQUEST_DAZUO_RESPONSE,
	GM_REQUEST_TIAOXI,												/**< �����Ϣ */
	GM_REQUEST_TIAOXI_RESPONSE,

	//-------------------------------------------------------------------------------------
	GM_REQUEST_MOVE = 102000,				//��������ƶ�
	GM_NOTIFY_MOVEING,						//�㲥����ƶ�
	GM_REQUEST_USE,							//����ʹ�õ���
	GM_USE_RESPONSE,	
	PLAYERMESSAGEEND,
};