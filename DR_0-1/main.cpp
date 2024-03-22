#define _USE_MATH_DEFINES
#include <Novice.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#define MATRIX_SIZE 3


const char kWindowTitle[] = "5103_Death_Revolver_ver1.0";

typedef struct Matrix3x3 {
	float m[3][3];
}Matrix3x3;

typedef struct Vector2 {
	float x;
	float y;
}Vector2;

typedef struct Player {
	Vector2 position;
	float width;
	float height;
	bool isAlive;
	bool isShoot;
}Player;

typedef struct Enemy {
	Vector2 position;
	float width;
	float height;
	bool isAlive;
	bool isShoot;
}Enemy;

typedef struct Bullet {
	Vector2 position;
	float width;
	float height;
	bool isExist;
}Bullet;

typedef struct Icon {
	Vector2 position;
	float width;
	float height;
}Icon;

int IsSpriteCollision(float sprite1PosX, float sprite1Width, float sprite1PosY, float sprite1Height, float sprite2PosX, float sprite2Width, float sprite2PosY, float sprite2Height) {
	if (sprite1PosX <= sprite2PosX + sprite2Width && sprite2PosX <= sprite1PosX + sprite1Width && sprite1PosY <= sprite2PosY + sprite2Height && sprite2PosY <= sprite1PosY + sprite1Height) {
		return 1;
	}
	else {
		return 0;
	}
}

float easeInSine(float x) {
	return 1.0f - cosf((x * (float)M_PI) / 2.0f);
}


Matrix3x3 MakeAffineMatrix(Vector2 scale, float rotate, Vector2 translate) {
	Matrix3x3 result;
	result.m[0][0] = scale.x * cosf(rotate);
	result.m[0][1] = scale.x * sinf(rotate);
	result.m[0][2] = 0;
	result.m[1][0] = -scale.y * sinf(rotate);
	result.m[1][1] = scale.y * cosf(rotate);
	result.m[1][2] = 0;
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	result.m[2][2] = 1;
	return result;
}


Vector2 Transform(Vector2 vector, Matrix3x3 matrix) {
	Vector2 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
	float w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	return result;
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	srand((unsigned)time(NULL));

	enum Scene {
		TITLE,
		TUTORIAL,
		STAGE1LOADING,
		STAGE1,
		STAGE2LOADING,
		STAGE2,
		CLEAR
	};

	Scene scene = TITLE;

	//音乐

	int SOUND_GunFire = Novice::LoadAudio("./Resources/Sounds/gun.wav");//枪声
	int SOUND_Boom = Novice::LoadAudio("./Resources/Sounds/boom.wav");//爆炸声
	//int SOUND_Wind = Novice::LoadAudio("./Resources/Sounds/wind.wav");//风声
	int SOUND_Hawk = Novice::LoadAudio("./Resources/Sounds/hawk.wav");//鹰叫声
	int SOUND_Bgm = Novice::LoadAudio("./Resources/Sounds/bgm.wav");//bgm
	int SOUND_TitleBgm = Novice::LoadAudio("./Resources/Sounds/titleBgm.wav");//bgm
	int SOUND_WinSound = Novice::LoadAudio("./Resources/Sounds/winSound.wav");//bgm

	int voiceHandle_GunFire = -1;
	int voiceHandle_Boom = -1;
	/*int voiceHandle_Wind = -1;*/
	int voiceHandle_Hawk = -1;
	int voiceHandle_Bgm = -1;
	int voiceHandle_TitleBgm = -1;
	int voiceHandle_WinSound = -1;

	//Title 画像
	int Texture_Title = Novice::LoadTexture("./Resources/Title/title_Sheet.png");
	int Texture_BG = Novice::LoadTexture("./Resources/Title/bg.png");
	int Texture_Box = Novice::LoadTexture("./Resources/Title/box.png");
	int Texture_Start = Novice::LoadTexture("./Resources/Title/start.png");
	int Texture_Clear = Novice::LoadTexture("./Resources/Title/clear.png");
	//loading
	int Texture_Wanted1 = Novice::LoadTexture("./Resources/Title/wanted1.png");
	int Texture_Wanted2 = Novice::LoadTexture("./Resources/Title/wanted2.png");




	//Title 动画计时器和计数器
	int titleTimer = 0;						//计时器，动画的播放时间
	int titleCounter = 0;					//计数器，用来计算三个弹孔动画的参数
	//Flag 
	bool isTitleAnimationStart = false;		//是否播放动画
	bool isTitleAnimationEnd = false;		//动画是否播放完毕

	//教程图片
	int Texture_Tutorial = Novice::LoadTexture("./Resources/Title/tutorial-Sheet.png");
	//参数
	int tutorialTimer = 0;						//计时器，动画的播放时间
	int tutorialCounter = 0;

	//人物画像

	int Texture_Player1 = Novice::LoadTexture("./Resources/Game/player1.png");
	int Texture_Player2 = Novice::LoadTexture("./Resources/Game/player2.png");//shoot
	int Texture_Player3 = Novice::LoadTexture("./Resources/Game/player3.png");//dead
	int Texture_Enemy = Novice::LoadTexture("./Resources/Game/enemy.png");
	int Texture_Enemy1 = Novice::LoadTexture("./Resources/Game/enemy1.png");//dead
	int Texture_Bullet = Novice::LoadTexture("./Resources/Game/bullet.png");
	int Texture_Gun = Novice::LoadTexture("./Resources/Game/icon_Gun.png");
	int Texture_Bomb = Novice::LoadTexture("./Resources/Game/icon_Bomb.png");

	int Texture_Win = Novice::LoadTexture("./Resources/Game/win.png");
	int Texture_Lose = Novice::LoadTexture("./Resources/Game/lose.png");

	int Texture_Boss = Novice::LoadTexture("./Resources/Game/boss.png");
	int Texture_Boss1 = Novice::LoadTexture("./Resources/Game/boss1.png");//dead

	//风滚草
	int Texture_Tumbleweed = Novice::LoadTexture("./Resources/Game/tumbleweed.png");


	//参数

	bool isWin = false;

	int texturePlayer1Counter = 0;
	int texturePlayer1Frame = 0;
	/*int texturePlayer2Counter = 0;
	int texturePlayer2Frame = 0;*/
	int texturePlayer3Counter = 0;
	int texturePlayer3Frame = 0;
	int textureEnemyCounter = 0;
	int textureEnemyFrame = 0;
	int textureBossCounter = 0;
	int textureBossFrame = 0;
	Player player = {
		{-150.0f,400.0f},
		128.0f,
		128.0f,
		true,
		false,
	};

	Enemy enemy = {
		{1281.0f,400.0f},
		128.0f,
		128.0f,
		true,
		false,
	};

	Enemy boss = {
		{1281.0f,400.0f},
		128.0f,
		128.0f,
		true,
		false,
	};
	int bossLife = 10;
	bool isBossStop = false;
	int bossStoptimer = 60;
	float bossMoveVelocity = 5.0f;
	Bullet bulletPlayer = {
		{-100.0f,-100.0f},
		64.0f,
		64.0f,
		false,
	};

	Icon iconGun = {
		{576.0f,580.0f},
		128.0f,
		128.0f,
	};
	Icon iconBomb = {
		{576.0f,580.0f},
		128.0f,
		128.0f,
	};
	bool isAnimationOn = true;
	float moveVelocity = 5.0f;
	int iconAppearTimer = -1;
	int iconType = 0;
	int iconTimer = 0;
	int iconCounter = 15;
	int gunNum = 0;
	bool isIconAppearTimerRandom = false;
	bool isIconTypeRandom = false;
	bool isIconTimer = false;
	bool isCounterDown = false;

	float endFrame = 60.0f;
	float nowFrame = 0.0f;
	float x = 0.0f;


	int loadingTimer1 = 60;
	int loadingTimer2 = 60;

	int spaceTimer = 0;
	bool isSpaceOn = false;

	//旋转移动
	float theta = 0;
	float width = 64.0f;
	float height = 64.0f;
	Vector2 rectCenter_WCS = { 0,-138 };
	Vector2 leftTop0_WCS = { -width / 2,height / 2 };
	Vector2 rightTop0_WCS = { width / 2,height / 2 };
	Vector2 leftBottom0_WCS = { -width / 2,-height / 2 };
	Vector2 rightBottom0_WCS = { width / 2,-height / 2 };
	Vector2 scale = { 1.0f,1.0f };

	Matrix3x3 affineMatrix = {};
	Vector2 worldLeftTop_WCS = {};
	Vector2 worldRightTop_WCS = {};
	Vector2 worldLeftBottom_WCS = {};
	Vector2 worldRightBottom_WCS = {};
	Vector2 worldLeftTop_SCS = {};
	Vector2 worldRightTop_SCS = {};
	Vector2 worldLeftBottom_SCS = {};
	Vector2 worldRightBottom_SCS = {};
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		if (player.isAlive) {
			texturePlayer1Counter++;
			if (texturePlayer1Counter % 15 == 0) {
				texturePlayer1Counter = 0;
				texturePlayer1Frame++;
				if (texturePlayer1Frame > 3) {
					texturePlayer1Frame = 0;
				}
			}
		}
		/*	if (player.isShoot) {
				texturePlayer2Counter++;
				if (texturePlayer2Counter % 15 == 0) {
					texturePlayer2Counter = 0;
					texturePlayer2Frame++;
					if (texturePlayer2Frame > 3) {
						texturePlayer2Frame = 0;
					}
				}
			}*/

		if (!player.isAlive) {
			texturePlayer3Counter++;
			if (texturePlayer3Counter % 15 == 0) {
				texturePlayer3Counter = 0;
				texturePlayer3Frame++;
				if (texturePlayer3Frame > 4) {
					texturePlayer3Frame = 4;
				}
			}
		}

		if (!enemy.isAlive) {
			textureEnemyCounter++;
			if (textureEnemyCounter % 15 == 0) {
				textureEnemyCounter = 0;
				textureEnemyFrame++;
				if (textureEnemyFrame > 4) {
					textureEnemyFrame = 4;
				}
			}
		}

		if (!boss.isAlive) {
			textureBossCounter++;
			if (textureBossCounter % 15 == 0) {
				textureBossCounter = 0;
				textureBossFrame++;
				if (textureBossFrame > 4) {
					textureBossFrame = 4;
				}
			}
		}

		if (scene == TITLE) {
			Novice::StopAudio(voiceHandle_Bgm);
			Novice::StopAudio(voiceHandle_WinSound);
			/*if (Novice::IsPlayingAudio(voiceHandle_Wind) == 0 || voiceHandle_Wind == -1) {
				voiceHandle_Wind = Novice::PlayAudio(SOUND_Wind, true, 3.0f);
			}*/
			if (Novice::IsPlayingAudio(voiceHandle_TitleBgm) == 0 || voiceHandle_TitleBgm == -1) {
				voiceHandle_TitleBgm = Novice::PlayAudio(SOUND_TitleBgm, true, 0.5f);
			}
			rectCenter_WCS.x += 5;

			if (rectCenter_WCS.x > 1300) {
				rectCenter_WCS.x = -100;
			}

			theta -= 1.0f / 30 * (float)M_PI;

			if (theta < -2 * (float)M_PI) {
				theta = 0;
			}

			affineMatrix = MakeAffineMatrix(scale, theta, rectCenter_WCS);

			worldLeftTop_WCS = Transform(leftTop0_WCS, affineMatrix);
			worldRightTop_WCS = Transform(rightTop0_WCS, affineMatrix);
			worldLeftBottom_WCS = Transform(leftBottom0_WCS, affineMatrix);
			worldRightBottom_WCS = Transform(rightBottom0_WCS, affineMatrix);

			worldLeftTop_SCS = {
				worldLeftTop_WCS.x,
				worldLeftTop_WCS.y * -1 + 500
			};
			worldRightTop_SCS = {
				worldRightTop_WCS.x,
				worldRightTop_WCS.y * -1 + 500
			};
			worldLeftBottom_SCS = {
				worldLeftBottom_WCS.x,
				worldLeftBottom_WCS.y * -1 + 500
			};
			worldRightBottom_SCS = {
				worldRightBottom_WCS.x,
				worldRightBottom_WCS.y * -1 + 500
			};





			if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
				isTitleAnimationStart = true;
			}
			if (isTitleAnimationStart) {
				if (titleTimer >= 150) {								//动画播放时间为130frame
					titleTimer = 0;
					titleCounter = 0;
					isTitleAnimationStart = false;
					isTitleAnimationEnd = true;
				}
				else {
					titleTimer++;
					if (titleTimer > 0 && titleTimer % 40 == 0) {		//每40frame计数器+1，反映在动画上是多一个弹孔
						titleCounter++;
						voiceHandle_GunFire = Novice::PlayAudio(SOUND_GunFire, false, 1.0f);
					}
				}
			}
			if (isTitleAnimationEnd) {

				scene = TUTORIAL;
				isTitleAnimationEnd = false;
			}
		}
		if (scene == TUTORIAL) {
			//Novice::StopAudio(voiceHandle_Wind);
			if (tutorialTimer >= 90) {
				tutorialTimer = 90;
			}
			else {
				tutorialTimer++;
			}
			if (tutorialTimer == 90) {
				tutorialCounter = 1;
			}

			if (tutorialCounter == 1 && keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
				//scene = STAGE1;
				scene = STAGE1LOADING;
				tutorialTimer = 0;
				tutorialCounter = 0;
			}
		}
		if (scene == STAGE1LOADING) {

			if (loadingTimer1 > 0) {
				loadingTimer1--;
			}
			else {
				loadingTimer1 = 0;
			}
			if (loadingTimer1 <= 0) {
				scene = STAGE1;
				loadingTimer1 = 60;
			}
		}
		if (scene == STAGE1) {
			Novice::StopAudio(voiceHandle_TitleBgm);
			if (!isAnimationOn) {

				if (player.isAlive) {
					if (isCounterDown) {
						if (iconCounter > 0) {
							iconCounter--;
						}
						else {
							isCounterDown = false;
							iconCounter = -1;
						}
					}
					if (iconCounter == 0) {
						isIconAppearTimerRandom = true;
					}
					if (isIconAppearTimerRandom) {
						iconAppearTimer = rand() % 286 + 15;
						isIconAppearTimerRandom = false;
					}
					else {
						if (iconAppearTimer > 0) {
							iconAppearTimer--;
						}
						if (iconAppearTimer == 0) {
							isIconTypeRandom = true;
							iconAppearTimer = -1;
						}
					}

					if (isIconTypeRandom) {
						iconType = rand() % 2 + 1;
						if (iconType == 2) {
							iconTimer = rand() % 46 + 30;
						}
						else if (iconType == 1) {
							iconTimer = rand() % 26 + 15;
						}
						isIconTypeRandom = false;
					}

					if (iconType == 1 && iconTimer >= 0) {//1 枪	
						isIconTimer = true;
					}
					else if (iconType == 2 && iconTimer >= 0) {//2 炸弹
						isIconTimer = true;
					}
					if (isIconTimer) {
						if (iconTimer > 0) {
							iconTimer--;
						}
						if (iconTimer == 0) {
							if (iconType == 2) {
								isIconAppearTimerRandom = true;
							}
							if (iconType == 1 && gunNum == 0) {
								bulletPlayer.position.x = enemy.position.x;
								bulletPlayer.position.y = enemy.position.y + 40.0f;
								enemy.isShoot = true;
							}
							isIconTimer = false;
							iconTimer = -1;
						}
					}
					if (iconTimer > 0 && iconType == 2) {//炸弹
						if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
							voiceHandle_Boom = Novice::PlayAudio(SOUND_Boom, false, 1.0f);
							player.isAlive = false;
						}
					}

					if (iconTimer > 0 && iconType == 1) {//枪
						if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
							if (!player.isShoot) {
								bulletPlayer.position.x = player.position.x;
								bulletPlayer.position.y = player.position.y + 40.0f;
								gunNum++;
								voiceHandle_GunFire = Novice::PlayAudio(SOUND_GunFire, false, 1.0f);
								if (gunNum > 6) {
									gunNum = 6;
								}
								player.isShoot = true;
							}
						}
					}

					if (player.isShoot) {
						bulletPlayer.isExist = true;
						bulletPlayer.position.x += 50;


					}

					if (enemy.isShoot) {
						if (!bulletPlayer.isExist) {
							voiceHandle_GunFire = Novice::PlayAudio(SOUND_GunFire, false, 1.0f);
						}
						bulletPlayer.isExist = true;
						bulletPlayer.position.x -= 50;

					}

					//当たり判定
					if (player.isShoot && bulletPlayer.isExist && enemy.isAlive) {
						if (IsSpriteCollision(bulletPlayer.position.x, bulletPlayer.width, bulletPlayer.position.y, bulletPlayer.height, enemy.position.x, enemy.width, enemy.position.y, enemy.height)) {
							enemy.isAlive = false;
							bulletPlayer.isExist = false;
							player.isShoot = false;
						}
					}
					if (enemy.isShoot && bulletPlayer.isExist) {
						if (IsSpriteCollision(bulletPlayer.position.x, bulletPlayer.width, bulletPlayer.position.y, bulletPlayer.height, player.position.x, player.width, player.position.y, player.height)) {
							player.isAlive = false;
							bulletPlayer.isExist = false;
							enemy.isShoot = false;
						}
					}
					if (!enemy.isAlive && textureEnemyFrame == 4) {
						if (nowFrame >= endFrame) {
							nowFrame = endFrame;
						}
						else {
							nowFrame++;
							x = nowFrame / endFrame;
						}
						if (x == 1 && keys[DIK_SPACE]) {

							isSpaceOn = true;
							if (isSpaceOn) {
								spaceTimer++;
							}

						}
						if (keys[DIK_SPACE] == 0) {
							isSpaceOn = false;
						}
						if (!isSpaceOn && spaceTimer > 0 && spaceTimer <= 60) {
							nowFrame = 0;

							texturePlayer1Counter = 0;
							texturePlayer1Frame = 0;
							/*texturePlayer2Counter = 0;
							texturePlayer2Frame = 0;*/
							texturePlayer3Counter = 0;
							texturePlayer3Frame = 0;
							textureEnemyCounter = 0;
							textureEnemyFrame = 0;

							player.position.x = -150.0f;
							player.position.y = 400.0f;
							player.isAlive = true;
							player.isShoot = false;

							bulletPlayer.position.x = 1281.0f;
							bulletPlayer.position.y = 400.0f;
							bulletPlayer.isExist = false;

							isAnimationOn = true;
							iconAppearTimer = -1;
							iconType = 0;
							iconTimer = 0;
							iconCounter = 15;
							gunNum = 0;
							isIconAppearTimerRandom = false;
							isIconTypeRandom = false;
							isIconTimer = false;
							isCounterDown = false;

							//scene = STAGE2;
							spaceTimer = 0;
							scene = STAGE2LOADING;
						}
						/*if (x == 1 &&keys[DIK_SPACE] ) {

							isSpaceOn = true;
							if (isSpaceOn) {
								spaceTimer++;
							}
						}
						if (keys[DIK_SPACE] == 0) {
							isSpaceOn = false;
						}*/
						else if (!isSpaceOn && spaceTimer > 60) {
							nowFrame = 0;

							texturePlayer1Counter = 0;
							texturePlayer1Frame = 0;
							/*	texturePlayer2Counter = 0;
								texturePlayer2Frame = 0;*/
							texturePlayer3Counter = 0;
							texturePlayer3Frame = 0;
							textureEnemyCounter = 0;
							textureEnemyFrame = 0;
							textureBossCounter = 0;
							textureBossFrame = 0;
							player.position.x = -150.0f;
							player.position.y = 400.0f;
							player.isAlive = true;
							player.isShoot = false;

							enemy.position.x = 1281.0f;
							enemy.position.y = 400.0f;
							enemy.isAlive = true;
							enemy.isShoot = false;

							boss.position.x = 1281.0f;
							boss.position.y = 400.0f;
							boss.isAlive = true;
							boss.isShoot = false;
							bossLife = 10;

							bulletPlayer.position.x = 1281.0f;
							bulletPlayer.position.y = 400.0f;
							bulletPlayer.isExist = false;

							isAnimationOn = true;
							iconAppearTimer = -1;
							iconType = 0;
							iconTimer = 0;
							iconCounter = 15;
							gunNum = 0;
							isIconAppearTimerRandom = false;
							isIconTypeRandom = false;
							isIconTimer = false;
							isCounterDown = false;

							spaceTimer = 0;
							scene = TITLE;
						}
					}
				}
				else if (!player.isAlive && texturePlayer3Frame == 4) {
					if (nowFrame >= endFrame) {
						nowFrame = endFrame;
					}
					else {
						nowFrame++;
						x = nowFrame / endFrame;
					}
					if (x == 1 && keys[DIK_SPACE]) {


						isSpaceOn = true;
						if (isSpaceOn) {
							spaceTimer++;
						}
					}
					if (keys[DIK_SPACE] == 0) {
						isSpaceOn = false;
					}
					if (!isSpaceOn && spaceTimer > 0 && spaceTimer <= 60) {
						nowFrame = 0;

						texturePlayer1Counter = 0;
						texturePlayer1Frame = 0;
						/*texturePlayer2Counter = 0;
						texturePlayer2Frame = 0;*/
						texturePlayer3Counter = 0;
						texturePlayer3Frame = 0;
						textureEnemyCounter = 0;
						textureEnemyFrame = 0;
						player.position.x = -150.0f;
						player.position.y = 400.0f;
						player.isAlive = true;
						player.isShoot = false;

						enemy.position.x = 1281.0f;
						enemy.position.y = 400.0f;
						enemy.isAlive = true;
						enemy.isShoot = false;

						bulletPlayer.position.x = 1281.0f;
						bulletPlayer.position.y = 400.0f;
						bulletPlayer.isExist = false;

						isAnimationOn = true;
						iconAppearTimer = -1;
						iconType = 0;
						iconTimer = 0;
						iconCounter = 15;
						gunNum = 0;
						isIconAppearTimerRandom = false;
						isIconTypeRandom = false;
						isIconTimer = false;
						isCounterDown = false;
						spaceTimer = 0;
					}

					/*if (x == 1 && keys[DIK_SPACE] ) {

						isSpaceOn = true;
						if (isSpaceOn) {
							spaceTimer++;
						}
					}
					if (keys[DIK_SPACE] == 0) {
						isSpaceOn = false;
					}*/
					else if (!isSpaceOn && spaceTimer > 60) {

						nowFrame = 0;

						texturePlayer1Counter = 0;
						texturePlayer1Frame = 0;
						/*	texturePlayer2Counter = 0;
							texturePlayer2Frame = 0;*/
						texturePlayer3Counter = 0;
						texturePlayer3Frame = 0;
						textureEnemyCounter = 0;
						textureEnemyFrame = 0;
						textureBossCounter = 0;
						textureBossFrame = 0;
						player.position.x = -150.0f;
						player.position.y = 400.0f;
						player.isAlive = true;
						player.isShoot = false;

						enemy.position.x = 1281.0f;
						enemy.position.y = 400.0f;
						enemy.isAlive = true;
						enemy.isShoot = false;

						boss.position.x = 1281.0f;
						boss.position.y = 400.0f;
						boss.isAlive = true;
						boss.isShoot = false;
						bossLife = 10;

						bulletPlayer.position.x = 1281.0f;
						bulletPlayer.position.y = 400.0f;
						bulletPlayer.isExist = false;

						isAnimationOn = true;
						iconAppearTimer = -1;
						iconType = 0;
						iconTimer = 0;
						iconCounter = 15;
						gunNum = 0;
						isIconAppearTimerRandom = false;
						isIconTypeRandom = false;
						isIconTimer = false;
						isCounterDown = false;

						spaceTimer = 0;

						scene = TITLE;

					}
				}

			}
			else {
				if (enemy.position.x <= 1002.0f) {
					enemy.position.x = 1002.0f;
				}
				else {
					enemy.position.x -= moveVelocity;
				}
				if (player.position.x >= 150.0f) {
					player.position.x = 150.0f;
				}
				else {
					player.position.x += moveVelocity;
				}
				if (player.position.x == 150.0f && enemy.position.x == 1002.0f) {
					if (Novice::IsPlayingAudio(voiceHandle_Hawk) == 0 || voiceHandle_Hawk == -1) {
						voiceHandle_Hawk = Novice::PlayAudio(SOUND_Hawk, false, 0.5f);
					}
					isAnimationOn = false;
					isCounterDown = true;
				}

			}
		}
		if (scene == STAGE2LOADING) {
			if (loadingTimer2 > 0) {
				loadingTimer2--;
			}
			else {
				loadingTimer2 = 0;
			}
			if (loadingTimer2 <= 0) {
				scene = STAGE2;
				loadingTimer2 = 60;
			}
		}
		if (scene == STAGE2) {
			if (Novice::IsPlayingAudio(voiceHandle_Bgm) == 0 || voiceHandle_Bgm == -1) {
				voiceHandle_Bgm = Novice::PlayAudio(SOUND_Bgm, true, 0.4f);
			}
			if (!isAnimationOn) {
				if (player.isAlive && boss.isAlive) {
					boss.position.x -= bossMoveVelocity;
					if (isBossStop || !player.isAlive) {
						bossMoveVelocity = 0;
					}
					else {
						bossMoveVelocity = 1;
					}
					if (isCounterDown) {
						if (iconCounter > 0) {
							iconCounter--;
						}
						else {
							isCounterDown = false;
							iconCounter = -1;
						}
					}
					if (iconCounter == 0) {
						isIconAppearTimerRandom = true;
					}
					if (isIconAppearTimerRandom) {
						iconAppearTimer = rand() % 31 + 30;
						isIconAppearTimerRandom = false;
					}
					else {
						if (iconAppearTimer > 0) {
							iconAppearTimer--;
						}
						if (iconAppearTimer == 0) {
							isIconTypeRandom = true;
							iconAppearTimer = -1;
						}
					}

					if (isIconTypeRandom) {
						iconType = rand() % 3;
						if (iconType == 2) {
							iconTimer = rand() % 21 + 15;
						}
						else if (iconType == 1 || iconType == 0) {
							iconTimer = rand() % 26 + 15;
						}
						isIconTypeRandom = false;
					}

					if ((iconType == 1 || iconType == 0) && iconTimer >= 0) {//1 枪	
						isIconTimer = true;
					}
					else if (iconType == 2 && iconTimer >= 0) {//2 炸弹
						isIconTimer = true;
					}
					if (isIconTimer) {
						if (iconTimer > 0) {
							iconTimer--;
						}
						if (iconTimer == 0) {
							if (iconType == 2) {
								isIconAppearTimerRandom = true;
							}
							if (iconType == 1 || iconType == 0) {
								isIconAppearTimerRandom = true;
							}

							isIconTimer = false;
							iconTimer = -1;
						}
					}
					if (iconTimer > 0 && iconType == 2) {//炸弹
						if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
							voiceHandle_Boom = Novice::PlayAudio(SOUND_Boom, false, 1.0f);
							player.isAlive = false;
						}
					}

					if (iconTimer > 0 && (iconType == 1 || iconType == 0)) {//枪
						if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
							if (!player.isShoot) {
								bulletPlayer.position.x = player.position.x;
								bulletPlayer.position.y = player.position.y + 40.0f;
								gunNum++;
								voiceHandle_GunFire = Novice::PlayAudio(SOUND_GunFire, false, 1.0f);
								if (gunNum > 6) {
									gunNum = 6;
								}
								player.isShoot = true;
							}
						}
					}

					if (player.isShoot) {
						bulletPlayer.isExist = true;
						bulletPlayer.position.x += 50;
					}



					//当たり判定
					if (player.isShoot && bulletPlayer.isExist && boss.isAlive) {
						if (IsSpriteCollision(bulletPlayer.position.x, bulletPlayer.width, bulletPlayer.position.y, bulletPlayer.height, boss.position.x, boss.width, boss.position.y, boss.height)) {
							isBossStop = true;
							bossLife--;
							bulletPlayer.isExist = false;
							player.isShoot = false;
							if (bossLife <= 0) {
								boss.isAlive = false;
							}
						}
					}
					if (boss.isAlive && isBossStop) {
						if (bossStoptimer > 0) {
							bossStoptimer--;
						}
						else {
							isBossStop = false;
						}
					}
					else {
						bossStoptimer = 60;
					}

					if (IsSpriteCollision(player.position.x, player.width, player.position.y, player.height, boss.position.x, boss.width, boss.position.y, boss.height)) {
						player.isAlive = false;
						bossMoveVelocity = 0.0f;
					}
				}
				else if (!boss.isAlive && textureBossFrame == 4) {
					isWin = true;
					scene = CLEAR;


				}
				else if (!player.isAlive && texturePlayer3Frame == 4) {
					if (nowFrame >= endFrame) {
						nowFrame = endFrame;
					}
					else {
						nowFrame++;
						x = nowFrame / endFrame;
					}
					if (x == 1 && keys[DIK_SPACE]) {

						isSpaceOn = true;
						if (isSpaceOn) {
							spaceTimer++;
						}

					}
					if (keys[DIK_SPACE] == 0) {
						isSpaceOn = false;
					}
					if (!isSpaceOn && spaceTimer > 0 && spaceTimer <= 60) {
						nowFrame = 0;

						texturePlayer1Counter = 0;
						texturePlayer1Frame = 0;
						/*	texturePlayer2Counter = 0;
							texturePlayer2Frame = 0;*/
						texturePlayer3Counter = 0;
						texturePlayer3Frame = 0;
						textureEnemyCounter = 0;
						textureEnemyFrame = 0;
						textureBossCounter = 0;
						textureBossFrame = 0;
						player.position.x = -150.0f;
						player.position.y = 400.0f;
						player.isAlive = true;
						player.isShoot = false;

						boss.position.x = 1281.0f;
						boss.position.y = 400.0f;
						boss.isAlive = true;
						boss.isShoot = false;
						bossLife = 10;

						bulletPlayer.position.x = 1281.0f;
						bulletPlayer.position.y = 400.0f;
						bulletPlayer.isExist = false;

						isAnimationOn = true;
						iconAppearTimer = -1;
						iconType = 0;
						iconTimer = 0;
						iconCounter = 15;
						gunNum = 0;
						isIconAppearTimerRandom = false;
						isIconTypeRandom = false;
						isIconTimer = false;
						isCounterDown = false;
						spaceTimer = 0;
					}

					/*if (x == 1 && keys[DIK_SPACE]) {

						isSpaceOn = true;
						if (isSpaceOn) {
							spaceTimer++;
						}
					}
					if (keys[DIK_SPACE] == 0) {
						isSpaceOn = false;
					}*/
					else if (!isSpaceOn && spaceTimer > 60) {
						nowFrame = 0;

						texturePlayer1Counter = 0;
						texturePlayer1Frame = 0;
						/*	texturePlayer2Counter = 0;
							texturePlayer2Frame = 0;*/
						texturePlayer3Counter = 0;
						texturePlayer3Frame = 0;
						textureEnemyCounter = 0;
						textureEnemyFrame = 0;
						textureBossCounter = 0;
						textureBossFrame = 0;
						player.position.x = -150.0f;
						player.position.y = 400.0f;
						player.isAlive = true;
						player.isShoot = false;

						enemy.position.x = 1281.0f;
						enemy.position.y = 400.0f;
						enemy.isAlive = true;
						enemy.isShoot = false;

						boss.position.x = 1281.0f;
						boss.position.y = 400.0f;
						boss.isAlive = true;
						boss.isShoot = false;
						bossLife = 10;

						bulletPlayer.position.x = 1281.0f;
						bulletPlayer.position.y = 400.0f;
						bulletPlayer.isExist = false;

						isAnimationOn = true;
						iconAppearTimer = -1;
						iconType = 0;
						iconTimer = 0;
						iconCounter = 15;
						gunNum = 0;
						isIconAppearTimerRandom = false;
						isIconTypeRandom = false;
						isIconTimer = false;
						isCounterDown = false;
						spaceTimer = 0;
						scene = TITLE;
					}

				}

			}
			else {
				if (boss.position.x <= 1002.0f) {
					boss.position.x = 1002.0f;
				}
				else {
					boss.position.x -= moveVelocity;
				}
				if (player.position.x >= 150.0f) {
					player.position.x = 150.0f;
				}
				else {
					player.position.x += moveVelocity;
				}
				if (player.position.x == 150.0f && boss.position.x == 1002.0f) {
					if (Novice::IsPlayingAudio(voiceHandle_Hawk) == 0 || voiceHandle_Hawk == -1) {
						voiceHandle_Hawk = Novice::PlayAudio(SOUND_Hawk, false, 0.5f);
					}
					isAnimationOn = false;
					isCounterDown = true;
				}
			}
		}
		if (scene == CLEAR) {
			Novice::StopAudio(voiceHandle_Bgm);
			if (isWin) {
				if (Novice::IsPlayingAudio(voiceHandle_WinSound) == 0 || voiceHandle_WinSound == -1) {
					voiceHandle_WinSound = Novice::PlayAudio(SOUND_WinSound, false, 1.0f);
				}
				isWin = false;
			}
			if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0) {
				nowFrame = 0;

				texturePlayer1Counter = 0;
				texturePlayer1Frame = 0;
				/*	texturePlayer2Counter = 0;
					texturePlayer2Frame = 0;*/
				texturePlayer3Counter = 0;
				texturePlayer3Frame = 0;
				textureEnemyCounter = 0;
				textureEnemyFrame = 0;
				textureBossCounter = 0;
				textureBossFrame = 0;
				player.position.x = -150.0f;
				player.position.y = 400.0f;
				player.isAlive = true;
				player.isShoot = false;

				enemy.position.x = 1281.0f;
				enemy.position.y = 400.0f;
				enemy.isAlive = true;
				enemy.isShoot = false;

				boss.position.x = 1281.0f;
				boss.position.y = 400.0f;
				boss.isAlive = true;
				boss.isShoot = false;
				bossLife = 10;

				bulletPlayer.position.x = 1281.0f;
				bulletPlayer.position.y = 400.0f;
				bulletPlayer.isExist = false;

				isAnimationOn = true;
				iconAppearTimer = -1;
				iconType = 0;
				iconTimer = 0;
				iconCounter = 15;
				gunNum = 0;
				isIconAppearTimerRandom = false;
				isIconTypeRandom = false;
				isIconTimer = false;
				isCounterDown = false;

				scene = TITLE;
			}
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
	/*	Novice::ScreenPrintf(0, 0, "iconCounter = %d", iconCounter);
		Novice::ScreenPrintf(0, 15, "isCounterDown = %d", isCounterDown);
		Novice::ScreenPrintf(0, 30, "isIconAppearTimerRandom = %d", isIconAppearTimerRandom);
		Novice::ScreenPrintf(0, 45, "gunNum = %d", gunNum);
		Novice::ScreenPrintf(0, 60, "player.position.x = %f", player.position.x);
		Novice::ScreenPrintf(0, 75, "enemy.position.x  = %f", enemy.position.x);
		Novice::ScreenPrintf(0, 90, "iconAppearTimer  = %d", iconAppearTimer);
		Novice::ScreenPrintf(0, 105, "iconType  = %d", iconType);
		Novice::ScreenPrintf(0, 120, "iconTimer  = %d", iconTimer);
		Novice::ScreenPrintf(0, 135, "isIconTypeRandom  = %d", isIconTypeRandom);
		Novice::ScreenPrintf(0, 150, "bossLife  = %d", bossLife);
		Novice::ScreenPrintf(0, 165, "isBossAlive  = %d", boss.isAlive);
		Novice::ScreenPrintf(0, 180, "textureBossFrame  = %d", textureBossFrame);
		Novice::ScreenPrintf(0, 195, "scene  = %d", scene);*/

		if (scene == TITLE) {
			//Title 背景图片描画
			Novice::DrawSpriteRect(0, 0, titleCounter * 1280, 0, 1280, 720, Texture_Title, 0.25f, 1, 0.0f, WHITE);
			Novice::DrawSprite(0, 0, Texture_Start, 1, 1, 0.0f, WHITE);

			Novice::DrawQuad(
				(int)worldLeftTop_SCS.x, (int)worldLeftTop_SCS.y,
				(int)worldRightTop_SCS.x, (int)worldRightTop_SCS.y,
				(int)worldLeftBottom_SCS.x, (int)worldLeftBottom_SCS.y,
				(int)worldRightBottom_SCS.x, (int)worldRightBottom_SCS.y,
				0, 0, 64, 64, Texture_Tumbleweed, WHITE
			);
		}
		if (scene == TUTORIAL) {
			Novice::DrawSpriteRect(0, 0, tutorialCounter * 1280, 0, 1280, 720, Texture_Tutorial, 0.5f, 1, 0.0f, WHITE);
		}
		if (scene == STAGE1) {
			//bg
			Novice::DrawSprite(0, 0, Texture_BG, 1, 1, 0.0f, WHITE);
			//待机动画
			if (player.isAlive && !player.isShoot) {
				Novice::DrawSpriteRect(
					(int)player.position.x,
					(int)player.position.y,
					128 * texturePlayer1Frame, 0,
					128, 128,
					Texture_Player1,
					1.0f / 4, 1,
					0.0f, WHITE
				);
			}
			//射击动画
			if (player.isAlive && player.isShoot) {

				Novice::DrawSprite(
					(int)player.position.x,
					(int)player.position.y,
					Texture_Player2,
					1, 1, 0.0f, WHITE
				);
			}
			//死亡动画
			if (!player.isAlive) {
				Novice::DrawSpriteRect(
					(int)player.position.x,
					(int)player.position.y,
					128 * texturePlayer3Frame, 0,
					128, 128,
					Texture_Player3,
					1.0f / 4, 1,
					0.0f, WHITE
				);
			}

			//敌人动画
			if (enemy.isAlive) {
				Novice::DrawSprite(
					(int)enemy.position.x,
					(int)enemy.position.y,
					Texture_Enemy,
					1, 1, 0.0f, WHITE
				);
			}
			if (!enemy.isAlive) {
				Novice::DrawSpriteRect(
					(int)enemy.position.x,
					(int)enemy.position.y,
					128 * textureEnemyFrame, 0,
					128, 128,
					Texture_Enemy1,
					1.0f / 4, 1,
					0.0f, WHITE
				);
			}
			//子弹描画
			if (bulletPlayer.isExist) {
				Novice::DrawSprite(
					(int)bulletPlayer.position.x,
					(int)bulletPlayer.position.y,
					Texture_Bullet,
					1, 1, 0.0f, WHITE
				);
			}
			//枪 icon
			if (player.isAlive && enemy.isAlive && iconType == 1 && iconTimer > 0) {
				Novice::DrawSprite(
					(int)iconGun.position.x,
					(int)iconGun.position.y,
					Texture_Gun,
					1, 1, 0.0f, WHITE
				);
			}
			//炸弹 icon
			if (player.isAlive && enemy.isAlive && iconType == 2 && iconTimer > 0) {
				Novice::DrawSprite(
					(int)iconBomb.position.x,
					(int)iconBomb.position.y,
					Texture_Bomb,
					1, 1, 0.0f, WHITE
				);
			}
			if (!player.isAlive && texturePlayer3Frame == 4) {
				Novice::DrawSprite(440, 210, Texture_Lose, 1/*easeInSine(x)*/, easeInSine(x), 0.0f, WHITE);
			}
			if (!enemy.isAlive && textureEnemyFrame == 4) {
				Novice::DrawSprite(440, 210, Texture_Win, 1/*easeInSine(x)*/, easeInSine(x), 0.0f, WHITE);
			}
			Novice::DrawSprite(473, 577, Texture_Box, 1, 1, 0.0f, WHITE);

		}
		if (scene == STAGE2) {
			//bg
			Novice::DrawSprite(0, 0, Texture_BG, 1, 1, 0.0f, WHITE);

			//待机动画
			if (player.isAlive && !player.isShoot) {
				Novice::DrawSpriteRect(
					(int)player.position.x,
					(int)player.position.y,
					128 * texturePlayer1Frame, 0,
					128, 128,
					Texture_Player1,
					1.0f / 4, 1,
					0.0f, WHITE
				);
			}
			//射击动画
			if (player.isAlive && player.isShoot) {

				Novice::DrawSprite(
					(int)player.position.x,
					(int)player.position.y,
					Texture_Player2,
					1, 1, 0.0f, WHITE
				);
			}
			//死亡动画
			if (!player.isAlive) {
				Novice::DrawSpriteRect(
					(int)player.position.x,
					(int)player.position.y,
					128 * texturePlayer3Frame, 0,
					128, 128,
					Texture_Player3,
					1.0f / 4, 1,
					0.0f, WHITE
				);
			}

			//敌人动画
			if (boss.isAlive) {
				Novice::DrawSprite(
					(int)boss.position.x,
					(int)boss.position.y,
					Texture_Boss,
					1, 1, 0.0f, WHITE
				);
			}
			if (!boss.isAlive) {
				Novice::DrawSpriteRect(
					(int)boss.position.x,
					(int)boss.position.y,
					128 * textureBossFrame, 0,
					128, 128,
					Texture_Boss1,
					1.0f / 4, 1,
					0.0f, WHITE
				);
			}
			//子弹描画
			if (bulletPlayer.isExist) {
				Novice::DrawSprite(
					(int)bulletPlayer.position.x,
					(int)bulletPlayer.position.y,
					Texture_Bullet,
					1, 1, 0.0f, WHITE
				);
			}
			//枪 icon
			if (player.isAlive && boss.isAlive && (iconType == 1 || iconType == 0) && iconTimer > 0) {
				Novice::DrawSprite(
					(int)iconGun.position.x,
					(int)iconGun.position.y,
					Texture_Gun,
					1, 1, 0.0f, WHITE
				);
			}
			//炸弹 icon
			if (player.isAlive && boss.isAlive && iconType == 2 && iconTimer > 0) {
				Novice::DrawSprite(
					(int)iconBomb.position.x,
					(int)iconBomb.position.y,
					Texture_Bomb,
					1, 1, 0.0f, WHITE
				);
			}
			if (!player.isAlive && texturePlayer3Frame == 4) {
				Novice::DrawSprite(440, 210, Texture_Lose, 1 /*easeInSine(x)*/, easeInSine(x), 0.0f, WHITE);
			}
			Novice::DrawSprite(473, 577, Texture_Box, 1, 1, 0.0f, WHITE);
		}
		if (scene == CLEAR) {
			//bg
			Novice::DrawSprite(0, 0, Texture_Clear, 1, 1, 0.0f, WHITE);
		}
		if (scene == STAGE1LOADING) {
			Novice::DrawSprite(0, 0, Texture_Wanted1, 1, 1, 0.0f, WHITE);
		}
		if (scene == STAGE2LOADING) {
			Novice::DrawSprite(0, 0, Texture_Wanted2, 1, 1, 0.0f, WHITE);
		}
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_BACKSPACE] == 0 && keys[DIK_BACKSPACE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
