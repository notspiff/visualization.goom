#pragma once

#include "vivid/types.h"
#include <vector>

#if __cplusplus > 201402L
namespace GOOM::UTILS::COLOR_DATA
#else
namespace GOOM
{
namespace UTILS
{
namespace COLOR_DATA
{
#endif

    // clang-format off
static const std::vector<vivid::srgb_t> magma
{
  {   0.00146f,   0.00047f,   0.01387f },
  {   0.00226f,   0.00129f,   0.01833f },
  {   0.00328f,   0.00231f,   0.02371f },
  {   0.00451f,   0.00349f,   0.02996f },
  {   0.00595f,   0.00484f,   0.03713f },
  {   0.00759f,   0.00636f,   0.04497f },
  {   0.00943f,   0.00802f,   0.05284f },
  {   0.01146f,   0.00983f,   0.06075f },
  {   0.01371f,   0.01177f,   0.06867f },
  {   0.01616f,   0.01384f,   0.07660f },
  {   0.01881f,   0.01603f,   0.08458f },
  {   0.02169f,   0.01832f,   0.09261f },
  {   0.02479f,   0.02072f,   0.10068f },
  {   0.02812f,   0.02320f,   0.10879f },
  {   0.03170f,   0.02576f,   0.11696f },
  {   0.03552f,   0.02840f,   0.12521f },
  {   0.03961f,   0.03109f,   0.13351f },
  {   0.04383f,   0.03383f,   0.14189f },
  {   0.04806f,   0.03661f,   0.15033f },
  {   0.05232f,   0.03941f,   0.15884f },
  {   0.05661f,   0.04216f,   0.16745f },
  {   0.06095f,   0.04479f,   0.17613f },
  {   0.06533f,   0.04732f,   0.18489f },
  {   0.06976f,   0.04973f,   0.19373f },
  {   0.07426f,   0.05202f,   0.20266f },
  {   0.07881f,   0.05418f,   0.21167f },
  {   0.08345f,   0.05622f,   0.22076f },
  {   0.08815f,   0.05813f,   0.22992f },
  {   0.09295f,   0.05990f,   0.23916f },
  {   0.09783f,   0.06153f,   0.24848f },
  {   0.10282f,   0.06301f,   0.25785f },
  {   0.10790f,   0.06434f,   0.26729f },
  {   0.11309f,   0.06549f,   0.27678f },
  {   0.11840f,   0.06648f,   0.28632f },
  {   0.12383f,   0.06729f,   0.29588f },
  {   0.12938f,   0.06793f,   0.30544f },
  {   0.13505f,   0.06839f,   0.31500f },
  {   0.14086f,   0.06865f,   0.32454f },
  {   0.14678f,   0.06874f,   0.33401f },
  {   0.15284f,   0.06864f,   0.34340f },
  {   0.15902f,   0.06835f,   0.35269f },
  {   0.16531f,   0.06791f,   0.36182f },
  {   0.17171f,   0.06731f,   0.37077f },
  {   0.17821f,   0.06658f,   0.37950f },
  {   0.18480f,   0.06573f,   0.38797f },
  {   0.19146f,   0.06482f,   0.39615f },
  {   0.19818f,   0.06386f,   0.40401f },
  {   0.20494f,   0.06291f,   0.41151f },
  {   0.21172f,   0.06199f,   0.41865f },
  {   0.21851f,   0.06116f,   0.42539f },
  {   0.22530f,   0.06044f,   0.43174f },
  {   0.23208f,   0.05989f,   0.43770f },
  {   0.23883f,   0.05952f,   0.44326f },
  {   0.24554f,   0.05935f,   0.44844f },
  {   0.25222f,   0.05942f,   0.45325f },
  {   0.25886f,   0.05971f,   0.45771f },
  {   0.26545f,   0.06024f,   0.46184f },
  {   0.27199f,   0.06099f,   0.46566f },
  {   0.27849f,   0.06198f,   0.46919f },
  {   0.28495f,   0.06317f,   0.47245f },
  {   0.29137f,   0.06455f,   0.47546f },
  {   0.29774f,   0.06612f,   0.47824f },
  {   0.30408f,   0.06784f,   0.48081f },
  {   0.31038f,   0.06970f,   0.48319f },
  {   0.31665f,   0.07169f,   0.48538f },
  {   0.32290f,   0.07378f,   0.48741f },
  {   0.32911f,   0.07597f,   0.48929f },
  {   0.33531f,   0.07824f,   0.49102f },
  {   0.34148f,   0.08056f,   0.49263f },
  {   0.34764f,   0.08295f,   0.49412f },
  {   0.35377f,   0.08537f,   0.49550f },
  {   0.35990f,   0.08783f,   0.49678f },
  {   0.36601f,   0.09031f,   0.49796f },
  {   0.37212f,   0.09282f,   0.49905f },
  {   0.37821f,   0.09533f,   0.50007f },
  {   0.38430f,   0.09785f,   0.50100f },
  {   0.39038f,   0.10038f,   0.50186f },
  {   0.39647f,   0.10290f,   0.50266f },
  {   0.40255f,   0.10542f,   0.50339f },
  {   0.40863f,   0.10793f,   0.50405f },
  {   0.41471f,   0.11043f,   0.50466f },
  {   0.42079f,   0.11292f,   0.50521f },
  {   0.42688f,   0.11539f,   0.50571f },
  {   0.43297f,   0.11786f,   0.50616f },
  {   0.43906f,   0.12030f,   0.50655f },
  {   0.44516f,   0.12272f,   0.50690f },
  {   0.45127f,   0.12513f,   0.50720f },
  {   0.45739f,   0.12752f,   0.50745f },
  {   0.46351f,   0.12989f,   0.50765f },
  {   0.46964f,   0.13225f,   0.50781f },
  {   0.47578f,   0.13458f,   0.50792f },
  {   0.48193f,   0.13689f,   0.50799f },
  {   0.48809f,   0.13919f,   0.50801f },
  {   0.49426f,   0.14146f,   0.50799f },
  {   0.50044f,   0.14372f,   0.50792f },
  {   0.50663f,   0.14596f,   0.50781f },
  {   0.51283f,   0.14818f,   0.50765f },
  {   0.51904f,   0.15038f,   0.50744f },
  {   0.52527f,   0.15257f,   0.50719f },
  {   0.53151f,   0.15474f,   0.50689f },
  {   0.53775f,   0.15689f,   0.50655f },
  {   0.54402f,   0.15903f,   0.50616f },
  {   0.55029f,   0.16116f,   0.50572f },
  {   0.55657f,   0.16327f,   0.50523f },
  {   0.56287f,   0.16537f,   0.50469f },
  {   0.56917f,   0.16745f,   0.50411f },
  {   0.57549f,   0.16953f,   0.50347f },
  {   0.58182f,   0.17160f,   0.50278f },
  {   0.58816f,   0.17365f,   0.50204f },
  {   0.59451f,   0.17570f,   0.50124f },
  {   0.60087f,   0.17774f,   0.50039f },
  {   0.60724f,   0.17978f,   0.49949f },
  {   0.61362f,   0.18181f,   0.49854f },
  {   0.62001f,   0.18384f,   0.49752f },
  {   0.62640f,   0.18587f,   0.49646f },
  {   0.63280f,   0.18789f,   0.49533f },
  {   0.63922f,   0.18992f,   0.49415f },
  {   0.64563f,   0.19195f,   0.49291f },
  {   0.65206f,   0.19399f,   0.49161f },
  {   0.65848f,   0.19603f,   0.49025f },
  {   0.66492f,   0.19808f,   0.48884f },
  {   0.67135f,   0.20013f,   0.48736f },
  {   0.67779f,   0.20220f,   0.48582f },
  {   0.68422f,   0.20429f,   0.48422f },
  {   0.69066f,   0.20638f,   0.48256f },
  {   0.69710f,   0.20850f,   0.48084f },
  {   0.70353f,   0.21064f,   0.47905f },
  {   0.70996f,   0.21280f,   0.47720f },
  {   0.71639f,   0.21498f,   0.47529f },
  {   0.72281f,   0.21719f,   0.47332f },
  {   0.72922f,   0.21944f,   0.47128f },
  {   0.73562f,   0.22171f,   0.46918f },
  {   0.74200f,   0.22403f,   0.46702f },
  {   0.74838f,   0.22638f,   0.46479f },
  {   0.75474f,   0.22877f,   0.46251f },
  {   0.76108f,   0.23121f,   0.46016f },
  {   0.76740f,   0.23370f,   0.45776f },
  {   0.77370f,   0.23625f,   0.45529f },
  {   0.77997f,   0.23885f,   0.45276f },
  {   0.78621f,   0.24151f,   0.45018f },
  {   0.79243f,   0.24424f,   0.44754f },
  {   0.79861f,   0.24704f,   0.44485f },
  {   0.80475f,   0.24991f,   0.44210f },
  {   0.81085f,   0.25286f,   0.43931f },
  {   0.81691f,   0.25589f,   0.43646f },
  {   0.82293f,   0.25902f,   0.43357f },
  {   0.82889f,   0.26223f,   0.43064f },
  {   0.83479f,   0.26554f,   0.42767f },
  {   0.84064f,   0.26895f,   0.42467f },
  {   0.84642f,   0.27247f,   0.42163f },
  {   0.85213f,   0.27611f,   0.41857f },
  {   0.85776f,   0.27986f,   0.41550f },
  {   0.86332f,   0.28373f,   0.41240f },
  {   0.86879f,   0.28773f,   0.40930f },
  {   0.87418f,   0.29186f,   0.40620f },
  {   0.87946f,   0.29613f,   0.40312f },
  {   0.88465f,   0.30053f,   0.40005f },
  {   0.88973f,   0.30508f,   0.39700f },
  {   0.89470f,   0.30977f,   0.39399f },
  {   0.89955f,   0.31462f,   0.39104f },
  {   0.90428f,   0.31961f,   0.38814f },
  {   0.90888f,   0.32476f,   0.38531f },
  {   0.91335f,   0.33005f,   0.38256f },
  {   0.91769f,   0.33550f,   0.37992f },
  {   0.92188f,   0.34110f,   0.37738f },
  {   0.92594f,   0.34684f,   0.37496f },
  {   0.92985f,   0.35273f,   0.37268f },
  {   0.93361f,   0.35876f,   0.37054f },
  {   0.93722f,   0.36493f,   0.36857f },
  {   0.94069f,   0.37122f,   0.36676f },
  {   0.94401f,   0.37764f,   0.36514f },
  {   0.94718f,   0.38418f,   0.36370f },
  {   0.95021f,   0.39082f,   0.36247f },
  {   0.95310f,   0.39756f,   0.36144f },
  {   0.95585f,   0.40440f,   0.36062f },
  {   0.95846f,   0.41132f,   0.36001f },
  {   0.96095f,   0.41832f,   0.35963f },
  {   0.96331f,   0.42539f,   0.35947f },
  {   0.96555f,   0.43252f,   0.35953f },
  {   0.96767f,   0.43970f,   0.35981f },
  {   0.96968f,   0.44694f,   0.36031f },
  {   0.97158f,   0.45421f,   0.36103f },
  {   0.97338f,   0.46152f,   0.36196f },
  {   0.97508f,   0.46886f,   0.36311f },
  {   0.97669f,   0.47623f,   0.36447f },
  {   0.97821f,   0.48361f,   0.36602f },
  {   0.97964f,   0.49101f,   0.36778f },
  {   0.98100f,   0.49843f,   0.36973f },
  {   0.98228f,   0.50585f,   0.37187f },
  {   0.98349f,   0.51328f,   0.37420f },
  {   0.98462f,   0.52071f,   0.37670f },
  {   0.98569f,   0.52815f,   0.37937f },
  {   0.98670f,   0.53558f,   0.38221f },
  {   0.98765f,   0.54302f,   0.38521f },
  {   0.98853f,   0.55045f,   0.38837f },
  {   0.98936f,   0.55787f,   0.39167f },
  {   0.99014f,   0.56530f,   0.39512f },
  {   0.99087f,   0.57271f,   0.39871f },
  {   0.99156f,   0.58011f,   0.40244f },
  {   0.99220f,   0.58750f,   0.40630f },
  {   0.99279f,   0.59489f,   0.41028f },
  {   0.99333f,   0.60228f,   0.41439f },
  {   0.99383f,   0.60964f,   0.41861f },
  {   0.99431f,   0.61700f,   0.42295f },
  {   0.99474f,   0.62435f,   0.42740f },
  {   0.99512f,   0.63170f,   0.43195f },
  {   0.99548f,   0.63903f,   0.43661f },
  {   0.99581f,   0.64634f,   0.44136f },
  {   0.99610f,   0.65366f,   0.44621f },
  {   0.99634f,   0.66097f,   0.45116f },
  {   0.99658f,   0.66826f,   0.45619f },
  {   0.99677f,   0.67554f,   0.46131f },
  {   0.99692f,   0.68283f,   0.46653f },
  {   0.99708f,   0.69009f,   0.47181f },
  {   0.99719f,   0.69735f,   0.47718f },
  {   0.99725f,   0.70461f,   0.48263f },
  {   0.99733f,   0.71185f,   0.48815f },
  {   0.99735f,   0.71909f,   0.49375f },
  {   0.99735f,   0.72632f,   0.49943f },
  {   0.99734f,   0.73355f,   0.50517f },
  {   0.99728f,   0.74077f,   0.51098f },
  {   0.99723f,   0.74798f,   0.51686f },
  {   0.99714f,   0.75519f,   0.52281f },
  {   0.99702f,   0.76240f,   0.52882f },
  {   0.99690f,   0.76959f,   0.53489f },
  {   0.99673f,   0.77680f,   0.54104f },
  {   0.99657f,   0.78398f,   0.54723f },
  {   0.99637f,   0.79117f,   0.55350f },
  {   0.99616f,   0.79835f,   0.55982f },
  {   0.99593f,   0.80553f,   0.56620f },
  {   0.99568f,   0.81271f,   0.57264f },
  {   0.99542f,   0.81988f,   0.57914f },
  {   0.99513f,   0.82705f,   0.58570f },
  {   0.99485f,   0.83421f,   0.59231f },
  {   0.99452f,   0.84139f,   0.59898f },
  {   0.99422f,   0.84854f,   0.60570f },
  {   0.99387f,   0.85571f,   0.61248f },
  {   0.99355f,   0.86286f,   0.61930f },
  {   0.99317f,   0.87002f,   0.62619f },
  {   0.99283f,   0.87717f,   0.63311f },
  {   0.99244f,   0.88433f,   0.64010f },
  {   0.99209f,   0.89147f,   0.64712f },
  {   0.99169f,   0.89863f,   0.65420f },
  {   0.99133f,   0.90576f,   0.66131f },
  {   0.99093f,   0.91292f,   0.66848f },
  {   0.99057f,   0.92005f,   0.67568f },
  {   0.99018f,   0.92720f,   0.68293f },
  {   0.98982f,   0.93433f,   0.69020f },
  {   0.98943f,   0.94147f,   0.69752f },
  {   0.98908f,   0.94860f,   0.70486f },
  {   0.98872f,   0.95574f,   0.71224f },
  {   0.98837f,   0.96288f,   0.71965f },
  {   0.98803f,   0.97001f,   0.72708f },
  {   0.98769f,   0.97715f,   0.73454f },
  {   0.98739f,   0.98429f,   0.74200f },
  {   0.98705f,   0.99144f,   0.74950f },
};
// clang-format on

#if __cplusplus > 201402L
} // namespace GOOM::UTILS::COLOR_DATA
#else
} // namespace COLOR_DATA
} // namespace UTILS
} // namespace GOOM
#endif
