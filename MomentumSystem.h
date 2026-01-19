#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

// Forward declarations
class Team;
class GameState;
class RivalryProfile;

// -----------------------------
// Enums / Types
// -----------------------------
enum class EventType
{
    Turnover,
    Sack,
    BigPlay,
    ThirdDownConversion,
    FourthDownConversion,
    MissedKick,
    Touchdown,
    Interception,
    Fumble,
    Stop,
    Other
};

// -----------------------------
// MomentumEvent
// -----------------------------
class MomentumEvent
{
public:
    MomentumEvent(EventType type, Team* sourceTeam, int severity, float timestamp);

    EventType GetType() const;
    Team* GetSourceTeam() const;
    int GetSeverity() const;
    float GetTimestamp() const;

private:
    EventType m_type;
    Team* m_sourceTeam;
    int m_severity;
    float m_timestamp;
};

// -----------------------------
// MomentumMeter
// -----------------------------
class MomentumMeter
{
public:
    MomentumMeter(int minValue = -100, int maxValue = 100, float decayRate = 0.25f);

    void Add(int points);
    void Decay(float deltaTime);
    void Clamp();

    int GetValue() const;

private:
    int m_value;
    int m_minValue;
    int m_maxValue;
    float m_decayRate;
};

// -----------------------------
// RivalryProfile
// -----------------------------
class RivalryProfile
{
public:
    RivalryProfile(const std::string& rivalryName = "",
                   int rivalryTier = 0,
                   float hostilityFactor = 1.0f,
                   bool isRivalryGame = false);

    const std::string& GetRivalryName() const;
    int GetRivalryTier() const;
    float GetHostilityFactor() const;
    bool IsRivalryGame() const;

private:
    std::string m_rivalryName;
    int m_rivalryTier;
    float m_hostilityFactor;
    bool m_isRivalryGame;
};

// -----------------------------
// Team
// -----------------------------
class Team
{
public:
    Team(const std::string& name = "", bool isHome = false, int disciplineRating = 50, int composureRating = 50);

    const std::string& GetName() const;
    bool IsHome() const;

    int GetDisciplineRating() const;
    int GetComposureRating() const;

    void SetDisciplineRating(int value);
    void SetComposureRating(int value);

private:
    std::string m_name;
    bool m_isHome;

    int m_disciplineRating;
    int m_composureRating;
};

// -----------------------------
// GameState
// -----------------------------
class GameState
{
public:
    GameState();

    int GetQuarter() const;
    int GetTimeRemaining() const;
    int GetScoreDiff() const;

    bool IsOnlineCompetitive() const;

    const RivalryProfile& GetRivalryProfile() const;
    void SetRivalryProfile(const RivalryProfile& profile);

    void SetQuarter(int quarter);
    void SetTimeRemaining(int seconds);
    void SetScoreDiff(int diff);
    void SetOnlineCompetitive(bool isOnline);

private:
    int m_quarter;
    int m_timeRemaining;
    int m_scoreDiff;
    bool m_isOnlineCompetitive;

    RivalryProfile m_rivalryProfile;
};

// -----------------------------
// MomentumRuleEngine
// -----------------------------
class MomentumRuleEngine
{
public:
    MomentumRuleEngine();

    int CalculateDelta(const MomentumEvent& event, const GameState& gameState) const;
    int ApplyRivalryMultiplier(int delta, const RivalryProfile& rivalry) const;

    void SetBasePoints(EventType type, int points);

private:
    std::unordered_map<EventType, int> m_basePoints;
};

// -----------------------------
// CrowdNoiseController
// -----------------------------
class CrowdNoiseController
{
public:
    CrowdNoiseController();

    void SetIntensity(float value);
    float GetIntensity() const;

    void PulseOnBigPlay();
    void ApplyHomeFieldAdvantage();

private:
    float m_crowdIntensity;
};

// -----------------------------
// Modifier (Abstract Base)
// -----------------------------
class Modifier
{
public:
    Modifier(const std::string& id, float strength, float duration);
    virtual ~Modifier() = default;

    const std::string& GetId() const;
    float GetStrength() const;
    float GetDuration() const;

    virtual void Apply(Team& team) = 0;
    virtual void Expire();

protected:
    std::string m_id;
    float m_strength;
    float m_duration;
};

// -----------------------------
// AccuracyModifier
// -----------------------------
class AccuracyModifier : public Modifier
{
public:
    AccuracyModifier(float strength, float duration);

    void Apply(Team& team) override;
};

// -----------------------------
// PenaltyRiskModifier
// -----------------------------
class PenaltyRiskModifier : public Modifier
{
public:
    PenaltyRiskModifier(float strength, float duration);

    void Apply(Team& team) override;
};

// -----------------------------
// GameplayModifierService
// -----------------------------
class GameplayModifierService
{
public:
    GameplayModifierService();

    void Apply(Team& team, int momentum, const GameState& gameState);
    void RemoveExpired();

    void AddModifier(std::shared_ptr<Modifier> modifier);
    void ClearModifiers();

private:
    std::vector<std::shared_ptr<Modifier>> m_activeModifiers;
};

// -----------------------------
// MomentumSystem
// -----------------------------
class MomentumSystem
{
public:
    MomentumSystem();

    void OnEvent(const MomentumEvent& event);
    void Update(const GameState& gameState, float deltaTime);

    int GetMomentum(const Team& team) const;

    void SetTeams(Team* homeTeam, Team* awayTeam);
    void SetRivalryProfile(const RivalryProfile& profile);

private:
    MomentumRuleEngine m_rules;

    MomentumMeter m_homeMeter;
    MomentumMeter m_awayMeter;

    GameplayModifierService m_modifierService;
    CrowdNoiseController m_crowdController;

    Team* m_homeTeam;
    Team* m_awayTeam;

    RivalryProfile m_rivalryProfile;
};
