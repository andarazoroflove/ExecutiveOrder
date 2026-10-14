#pragma once
#include "Common.h"
#include "InputManager.h"
#include <vector>
#include <string>

enum FighterState {
    FSTATE_IDLE,
    FSTATE_WALK_FWD,
    FSTATE_WALK_BACK,
    FSTATE_JUMP,
    FSTATE_CROUCH,
    FSTATE_BLOCK_HIGH,
    FSTATE_BLOCK_LOW,
    FSTATE_HITSTUN,
    FSTATE_BLOCKSTUN,
    FSTATE_KNOCKDOWN_AIR,
    FSTATE_KNOCKDOWN_GROUND,
    FSTATE_WAKEUP,
    FSTATE_ATTACK,
    FSTATE_SPECIAL,
    FSTATE_TAUNT,
    FSTATE_VICTORY,
    FSTATE_DEFEAT
};

enum HighBenchStage {
    HB_STAGE_THOMAS = 0,
    HB_STAGE_ROBERTS = 1,
    HB_STAGE_SOTOMAYOR = 2
};

struct AttackData {
    std::string name = "Attack";
    int startupFrames = 4;
    int activeFrames = 4;
    int recoveryFrames = 8;
    int damage = 10;
    AttackType type = ATTACK_MID;
    int hitstopFrames = 8;
    int hitstunFrames = 14;
    int blockstunFrames = 10;
    float pushback = 3.5f;
    bool causesKnockdown = false;
    bool isHeavy = false;
    float reachX = 30.0f;
    float reachY = 0.0f;
    float boxW = 28.0f;
    float boxH = 16.0f;
    bool isSpecial = false;
    AttackMoveType moveType = MOVE_LIGHT_PUNCH;
};

struct Projectile {
    Vector2 pos;
    Vector2 vel;
    float width = 18.0f;
    float height = 14.0f;
    int damage = 12;
    int lifetime = 180;
    bool active = false;
    bool facingRight = true;
    int type = 0;
    float bounceY = 0.0f;
};

struct CosmeticPuff {
    Vector2 pos{ 0.0f, 0.0f };
    float timer = 0.0f;
    float maxTimer = 0.5f;
    float baseSize = 8.0f;
};

struct BufferedInput {
    int button = 0;
    SpecialMotionType motion = MOTION_NONE;
    int motionButton = 0;
    bool isCrouch = false;
    int timer = 0;
};

class Fighter {
public:
    Fighter(int playerIndex, CharacterId charId, float startX);

    void ResetRound(float startX, bool faceRight, bool wasKnockedOut = false);
    void UpdatePhysics(const InputManager& input, const Fighter& opponent);
    void UpdateAnimation();

    bool CheckCanBlock(AttackType type) const;
    void ApplyHit(int damage, AttackType type, int hitstop, int hitstun, float pushback, bool knockdown);
    void ApplyBlock(int damage, int blockstop, int blockstun, float pushback);

    Rectangle GetPushbox() const;
    std::vector<Rectangle> GetHurtboxes() const;
    bool GetActiveHitbox(Rectangle& outBox, AttackData& outData) const;

    std::vector<Projectile>& GetProjectiles() { return m_projectiles; }
    const std::vector<Projectile>& GetProjectiles() const { return m_projectiles; }

    std::vector<CosmeticPuff>& GetCosmeticPuffs() { return m_cosmeticPuffs; }
    const std::vector<CosmeticPuff>& GetCosmeticPuffs() const { return m_cosmeticPuffs; }

    int GetPlayerIndex() const { return m_playerIndex; }
    CharacterId GetCharacterId() const { return m_charId; }
    HighBenchStage GetHighBenchStage() const {
        if (m_health > 66) return HB_STAGE_THOMAS;
        if (m_health > 33) return HB_STAGE_ROBERTS;
        return HB_STAGE_SOTOMAYOR;
    }
    std::string GetName() const {
        if (m_charId == CHAR_HIGH_BENCH) {
            HighBenchStage stage = GetHighBenchStage();
            if (stage == HB_STAGE_THOMAS) return "C. THOMAS";
            if (stage == HB_STAGE_ROBERTS) return "J. ROBERTS";
            return "S. SOTOMAYOR";
        }
        return m_name;
    }
    Vector2 GetPosition() const { return m_pos; }
    void SetPositionX(float x) { m_pos.x = x; }
    Vector2 GetVelocity() const { return m_vel; }
    bool IsFacingRight() const { return m_facingRight; }
    void SetFacingRight(bool fr) { m_facingRight = fr; }
    FighterState GetState() const { return m_state; }
    void SetState(FighterState st) { m_state = st; m_stateFrame = 0; }
    void TickWakeup() { if (m_state == FSTATE_WAKEUP) { m_stateFrame++; if (m_stateFrame >= 24) { m_state = FSTATE_IDLE; m_stateFrame = 0; } } }
    int GetHealth() const { return m_health; }
    int GetRefillCount() const { return m_refillCount; }
    int GetRoundsWon() const { return m_roundsWon; }
    void AddRoundWin() { m_roundsWon++; }
    bool IsInHitstop() const { return m_hitstopFrames > 0; }
    void SetHitstop(int frames) { m_hitstopFrames = frames; }
    int GetHitstopFrames() const { return m_hitstopFrames; }
    int GetStateFrame() const { return m_stateFrame; }
    bool HasHitConnected() const { return m_hitConnected; }
    void SetHitConnected(bool connected) { m_hitConnected = connected; }
    bool IsCrouching() const { return m_state == FSTATE_CROUCH || m_state == FSTATE_BLOCK_LOW; }
    bool IsAirborne() const { return m_pos.y < GROUND_Y; }
    bool IsHoldingBack() const { return m_isHoldingBack; }
    bool IsHoldingDownBack() const { return m_isHoldingDownBack; }
    const AttackData& GetCurrentAttack() const { return m_currentAttack; }
    int GetSpecialIndex() const { return m_specialIndex; }
    int GetComboCount() const { return m_comboCount; }
    void AddComboHit() { m_comboCount++; m_comboResetTimer = 50; }
    void ResetCombo() { m_comboCount = 0; m_comboResetTimer = 0; }
    int GetLastFrameAdvantage() const { return m_lastFrameAdvantage; }
    void SetLastFrameAdvantage(int adv) { m_lastFrameAdvantage = adv; }
    bool IsSuperMoveActive() const { return m_isSuperMove; }
    bool PopSuperFlashTrigger() { bool v = m_triggerSuperFlash; m_triggerSuperFlash = false; return v; }
    bool PopCancelCallout() { bool v = m_justCanceled; m_justCanceled = false; return v; }
    bool PopReversalCallout() { bool v = m_justReversal; m_justReversal = false; return v; }

    Color GetPrimaryColor() const { return m_color1; }
    Color GetSecondaryColor() const { return m_color2; }
    Color GetSkinColor() const { return m_colorSkin; }

public:
    void StartNormalAttack(AttackMoveType moveType, const char* name, int startup, int active, int recovery,
                           int damage, AttackType type, int hitstop, int hitstun,
                           int blockstun, float push, bool heavy, float reachX, float reachY, float bw, float bh);
    void StartNormalAttack(const char* name, int startup, int active, int recovery,
                           int damage, AttackType type, int hitstop, int hitstun,
                           int blockstun, float push, bool heavy, float reachX, float reachY, float bw, float bh) {
        StartNormalAttack(heavy ? MOVE_HEAVY_PUNCH : MOVE_LIGHT_PUNCH, name, startup, active, recovery,
                          damage, type, hitstop, hitstun, blockstun, push, heavy, reachX, reachY, bw, bh);
    }
    void ExecuteSpecial(SpecialMotionType motion, int buttonUsed, const Fighter& opponent);
    void TriggerSignatureSpecial(const Fighter& opponent);
    void TriggerTaunt();
    bool HasSpeechTaunt() const;
    const char* GetTauntQuote() const;
    int GetTauntTimer() const { return m_tauntTimer; }
    const std::string& GetActiveTaunt() const { return m_activeTaunt; }

private:
    void InitCharacterProfile();
    void SpawnProjectile(int type, float vx, float vy, int damage);
    bool TryExecuteBufferedMove(const Fighter& opponent);

    int m_playerIndex = 0;
    CharacterId m_charId = CHAR_COMMANDER;
    std::string m_name = "POTUS";

    Vector2 m_pos{ 0.0f, GROUND_Y };
    Vector2 m_vel{ 0.0f, 0.0f };
    bool m_facingRight = true;

    FighterState m_state = FSTATE_IDLE;
    int m_stateFrame = 0;
    int m_hitstopFrames = 0;
    int m_hitstunFrames = 0;
    int m_blockstunFrames = 0;
    float m_pushbackVel = 0.0f;
    bool m_hitConnected = false;
    int m_armor = 0;
    int m_comboCount = 0;
    int m_comboResetTimer = 0;
    int m_lastFrameAdvantage = 0;
    int m_invincibleFrames = 0;
    bool m_isSuperMove = false;
    bool m_triggerSuperFlash = false;
    bool m_justCanceled = false;
    bool m_justReversal = false;
    int m_multiHitsRemaining = 0;

    bool m_isHoldingBack = false;
    bool m_isHoldingDownBack = false;
    BufferedInput m_buffer{};

    int m_health = 100;
    int m_roundsWon = 0;
    int m_refillCount = 0;

    AttackData m_currentAttack{};
    int m_specialIndex = 0;
    std::vector<Projectile> m_projectiles;
    int m_quirkTimer = 0;
    std::vector<CosmeticPuff> m_cosmeticPuffs;
    int m_tauntTimer = 0;
    std::string m_activeTaunt = "";

    float m_walkFwdSpeed = 2.30f;
    float m_walkBackSpeed = 1.70f;
    float m_jumpSpeedY = -9.2f;
    float m_jumpSpeedX = 2.3f;
    float m_gravity = 0.48f;

    Color m_color1{ 20, 40, 140, 255 };
    Color m_color2{ 180, 20, 20, 255 };
    Color m_colorSkin{ 240, 190, 150, 255 };
};
