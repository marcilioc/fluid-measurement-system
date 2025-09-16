#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include <vector>
#include <functional>

// Definição dos estados do seu sistema
enum class SystemState {
    IDLE,              // System idle
    E1_DRYING,         // E1 Drying / E2 Idle
    E1_PURGING,        // E1 Purging / E2 Idle
    E1_REGENERATING,   // E1 Regenerating / E2 Drying
    E1_CONDITIONING,   // E1 Conditioning / E2 Drying
    E2_DRYING,         // E1 Idle / E2 Drying
    E2_PURGING,        // E1 Idle / E2 Purging
    E2_REGENERATING,   // E1 Drying / E2 Regenerating
    E2_CONDITIONING,   // E1 Drying / E2 Conditioning
    ERROR              // Error state
};

// Estrutura para definir condições de transição
struct StateCondition {
    // Condição de tempo
    bool hasTimeCondition;
    unsigned long timeThreshold;    // Tempo mínimo no estado (ms)
    
    // Condição booleana
    bool hasBooleanCondition;
    bool booleanThreshold;          // Valor esperado (true/false)

    // Condição de sensor
    bool hasSensorCondition;
    float sensorThreshold;          // Valor de comparação
    bool greaterThan;               // true = >, false = <
};

// Estrutura para definir uma transição
struct StateTransition {
    SystemState fromState;
    SystemState toState;
    StateCondition condition;
    std::function<void()> onTransition;  // Executado quando transição ocorre
};

class StateMachine {
private:
    SystemState currentState;
    SystemState previousState;
    unsigned long stateEntryTime;        // Quando entrou no estado atual
    unsigned long lastStateCheck;        // Controle de throttle
    
    std::vector<StateTransition> transitions;
    std::function<void(SystemState, SystemState)> stateChangeCallback;
    
    void changeState(SystemState newState);
    bool checkCondition(const StateCondition& condition, const float weight, const bool booleanStatus);
    void setupDefaultTransitions();
    
public:
    StateMachine();
    void init();
    void update(const float weight1, const bool booleanStatus);
    void forceState(SystemState newState);  // Para comandos MQTT
    
    // Getters
    SystemState getCurrentState() const { return currentState; }
    SystemState getPreviousState() const { return previousState; }
    unsigned long getTimeInCurrentState() const { return millis() - stateEntryTime; }
    String getStateString() const { return getStateString(currentState); }
    String getStateString(SystemState state) const;
    
    // Configuração
    void addTransition(SystemState from, SystemState to, StateCondition condition, std::function<void()> onTransition = nullptr);
    void setStateChangeCallback(std::function<void(SystemState, SystemState)> callback);
    
    // Métodos helper para criar transições facilmente
    void addTimeTransition(SystemState from, SystemState to, unsigned long timeMs, std::function<void()> transition = nullptr);
    void addBooleanTransition(SystemState from, SystemState to, bool status, bool expectedValue, std::function<void()> transition = nullptr);
    void addWeightTransition(SystemState from, SystemState to, float weight, bool greaterThan, std::function<void()> transition = nullptr);
};

#endif
