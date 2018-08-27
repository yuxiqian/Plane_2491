#ifndef CONST_CPP
#define CONST_CPP

#define APPLICATION_VERSION "1.0 Release"

#if defined(Q_OS_WIN32)
#define DPI_SCALE_FIX 0.5
#define OS_TYPE " for Windows"
#define random() rand()
#else
#define DPI_SCALE_FIX 1.0
#define OS_TYPE "for macOS"
#endif

#define ARC_TO_ANG 57.2

#define RES_PIXELS 100
#define FRAME_WIDTH 800
#define FRAME_HEIGHT 600

#define PLANE_Y_LIMIT 300

#define MAX_SCORE 2000000000

#define PLANE_GOTHIT 1
#define SHOOT_BULLET 2
#define PLANE_EXPLOSION 3
#define GOT_GIFT 4

#define MAP_HEIGHT 2000
#define MOVE_ACC 4
#define MOVE_DEACC 2
#define MAX_SPEED 17

#define MOVE_PIXELS_PER_TICK 10
#define PLANE_MOVEMENT_SPEED 20

//#define SCALE_CONSTANT 3000.0

#define SCORE_PUNISH_PER_HIT 500

#define LOW_BLOOD_WARNING 20

#define BULLET_COST 5000
#define ASSIST_PLANE_COST 80000

#define BLOOD_DEDUCTION_PER_HIT 5
#define TICK_TIME_MS 30
#define BONUS_SCORE 20000
#define ENEMY_GENERATE_POSSIBILITY 0.1
#define PRESENT_POSSIBILITY 0.001

#endif // CONST_CPP
