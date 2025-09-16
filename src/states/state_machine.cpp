#include "state_machine.h"
#include "config.h"

StateMachine::StateMachine() : 
    currentState(SystemState::IDLE),
    previousState(SystemState::IDLE),
    stateEntryTime(0),
    lastStateCheck(0) {
}

void StateMachine::init() {
    currentState = SystemState::IDLE;
    previousState = SystemState::IDLE;
    stateEntryTime = millis();
    lastStateCheck = millis();
    
    // Configurar transições padrão
    setupDefaultTransitions();
    
    Serial.println("State Machine started");
    Serial.println("Estado inicial: " + getStateString(currentState));
}

void StateMachine::setupDefaultTransitions() {
    Serial.println("Configurando transições padrão...");
    
    // IDLE -> E1_DRYING after start command
    addBooleanTransition(SystemState::IDLE, SystemState::E1_DRYING, true, true, []()
    {
        Serial.println("Iniciando ciclo de secagem no E1");
        digitalWrite(PUMP1, LOW);
        digitalWrite(PUMP2, HIGH);
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, HIGH);
        digitalWrite(PURGE_LED, LOW);
        Serial.println("E1 habilitado");
    });
    
    // E1_DRYING -> E1_PURGING quando peso >= 1.4kg
    addWeightTransition(SystemState::E1_DRYING, SystemState::E1_PURGING, 1.4, true, []()
    {
        Serial.println("Peso atingiu 1.4kg, iniciando purga no E1");
        digitalWrite(PUMP1, HIGH);
        digitalWrite(PUMP2, HIGH);
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, HIGH);
        Serial.println("E1 desabilitado");
        digitalWrite(PURGE_LED, HIGH);
        Serial.println("Purga iniciada");
    });
    
    // E1_PURGING -> E1_REGENERATING aapós 2 segundos
    addTimeTransition(SystemState::E1_PURGING, SystemState::E1_REGENERATING, 2000, []()
    {
        digitalWrite(PURGE_LED, LOW);
        Serial.println("Purga completa");
        digitalWrite(SLND1, LOW);
        digitalWrite(SLND2, HIGH);
        Serial.println("Iniciando regeneração no E1");
        Serial.println("Iniciando ciclo de secagem no E2");
        digitalWrite(PUMP1, HIGH);
        digitalWrite(PUMP2, LOW);
        Serial.println("E2 habilitado");
    });
    
    // E1_REGENERATING -> E1_CONDITIONING se peso < 1.0kg
    addWeightTransition(SystemState::E1_REGENERATING, SystemState::E1_CONDITIONING, 1.0, false, []()
    {
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, HIGH);
        digitalWrite(PUMP1, HIGH);
        digitalWrite(PUMP2, LOW);
        Serial.println("E1 regenerado");
        digitalWrite(E1_COND_LED, HIGH);
        Serial.println("Iniciando condicionamento em E1");
    });
    
    // E1_CONDITIONING -> E2_DRYING após 2 segundos
    addTimeTransition(SystemState::E1_CONDITIONING, SystemState::E2_DRYING, 2000, []()
    {
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, HIGH);
        digitalWrite(PUMP1, HIGH);
        digitalWrite(PUMP2, LOW);
        digitalWrite(E1_COND_LED, LOW);
        Serial.println("Condicionamento em E1 finalizado");
    });

    // E2_DRYING -> E2_PURGING quando peso >= 1.4kg
    addWeightTransition(SystemState::E2_DRYING, SystemState::E2_PURGING, 1.4, true, []()
    {
        Serial.println("Peso atingiu 1.4kg, iniciando purga no E2");
        digitalWrite(PUMP1, HIGH);
        digitalWrite(PUMP2, HIGH);
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, HIGH);
        Serial.println("E2 desabilitado");
        digitalWrite(PURGE_LED, HIGH);
        Serial.println("Purga iniciada");
    });

    // E2_PURGING -> E2_REGENERATING após 2 segundos
    addTimeTransition(SystemState::E2_PURGING, SystemState::E2_REGENERATING, 2000, []()
    {
        digitalWrite(PURGE_LED, LOW);
        Serial.println("Purga completa");
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, LOW);
        Serial.println("Iniciando regeneração no E2");
        Serial.println("Iniciando ciclo de secagem no E1");
        digitalWrite(PUMP1, LOW);
        digitalWrite(PUMP2, HIGH);
        Serial.println("E1 habilitado");
    });

    // E2_REGENERATING -> E2_CONDITIONING se peso < 1.0kg
    addWeightTransition(SystemState::E2_REGENERATING, SystemState::E2_CONDITIONING, 1.0, false, []()
    {
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, HIGH);
        digitalWrite(PUMP1, LOW);
        digitalWrite(PUMP2, HIGH);
        Serial.println("E2 regenerado");
        digitalWrite(E2_COND_LED, HIGH);
        Serial.println("Iniciando condicionamento em E2");
    });

    // E2_CONDITIONING -> E1_DRYING após 2 segundos
    addTimeTransition(SystemState::E2_CONDITIONING, SystemState::E1_DRYING, 2000, []()
    {
        digitalWrite(SLND1, HIGH);
        digitalWrite(SLND2, HIGH);
        digitalWrite(PUMP1, LOW);
        digitalWrite(PUMP2, HIGH);
        digitalWrite(E2_COND_LED, LOW);
        Serial.println("Condicionamento em E2 finalizado");
    });
    
    Serial.println(String(transitions.size()) + " transições configuradas");
}

void StateMachine::update(const float weight, const bool booleanStatus) {
    // Throttle: verificar apenas a cada 200ms
    if (millis() - lastStateCheck < 200) return;
    lastStateCheck = millis();
    
    // Verificar todas as transições possíveis do estado atual
    for (const auto& transition : transitions) {
        if (transition.fromState != currentState) continue;
        
        if (checkCondition(transition.condition, weight, booleanStatus)) {
            // Executar callback da transição (se existe)
            if (transition.onTransition) {
                transition.onTransition();
            }
            
            // Mudar para o novo estado
            changeState(transition.toState);
            break; // Processar apenas uma transição por ciclo
        }
    }
}

bool StateMachine::checkCondition(const StateCondition& condition, const float weight, const bool booleanStatus) {
    bool result = true;
    
    // Verificar condição de tempo
    if (condition.hasTimeCondition) {
        unsigned long timeInState = getTimeInCurrentState();
        result &= (timeInState >= condition.timeThreshold);
        
        // Debug para condições de tempo
        if (result && condition.hasTimeCondition && !condition.hasSensorCondition) {
            Serial.println("Condição de tempo atendida: " + String(timeInState) + "ms >= " + String(condition.timeThreshold) + "ms");
        }
    }
    
    // Verificar condição booleana
    if (condition.hasBooleanCondition) {
        // Para este exemplo, vamos assumir que o booleano é sempre true
        bool currentBooleanState = booleanStatus;
        
        result &= (currentBooleanState == condition.booleanThreshold);
        
        // Debug para condições booleanas
        if (result) {
            Serial.println("Condição booleana atendida: estado atual é " + String(currentBooleanState ? "true" : "false") + 
            ", esperado: " + String(condition.booleanThreshold ? "true" : "false"));
        }
    }

    // Verificar condição de sensor
    if (condition.hasSensorCondition) {
        float sensorValue = 0;
        sensorValue = weight;
        
        bool sensorConditionMet;
        if (condition.greaterThan) {
            sensorConditionMet = (sensorValue >= condition.sensorThreshold);
        } else {
            sensorConditionMet = (sensorValue < condition.sensorThreshold);
        }
        
        result &= sensorConditionMet;
        
        // Debug para condições de sensor
        if (sensorConditionMet) {
            Serial.println("Condição de sensor atendida: " + String(sensorValue) + 
            (condition.greaterThan ? " >= " : " < ") + String(condition.sensorThreshold));
        }
    }
    
    return result;
}

void StateMachine::changeState(SystemState newState) {
    if (newState == currentState) return;
    
    previousState = currentState;
    currentState = newState;
    stateEntryTime = millis();
    
    Serial.println("MUDANÇA DE ESTADO: " + getStateString(previousState) + " -> " + getStateString(currentState));
    
    // Chamar callback se configurado
    if (stateChangeCallback) {
        stateChangeCallback(previousState, currentState);
    }
}

void StateMachine::forceState(SystemState newState) {
    Serial.println("FORÇANDO estado para: " + getStateString(newState));
    changeState(newState);
}

String StateMachine::getStateString(SystemState state) const {
    switch (state) {
        case SystemState::IDLE:            return "IDLE";
        case SystemState::E1_DRYING:       return "E1_DRYING ";
        case SystemState::E1_PURGING:      return "E1_PURGING";
        case SystemState::E1_REGENERATING: return "E1_REGENERATING";
        case SystemState::E1_CONDITIONING: return "E1_CONDITIONING";
        case SystemState::E2_DRYING:       return "E2_DRYING";
        case SystemState::E2_PURGING:      return "E2_PURGING";
        case SystemState::E2_REGENERATING: return "E2_REGENERATING";
        case SystemState::E2_CONDITIONING: return "E2_CONDITIONING";
        case SystemState::ERROR:           return "ERROR";
        default:                           return "UNKNOWN";
    }
}

void StateMachine::addTransition(SystemState from, SystemState to, StateCondition condition, 
                                std::function<void()> onTransition) {
    StateTransition transition;
    transition.fromState = from;
    transition.toState = to;
    transition.condition = condition;
    transition.onTransition = onTransition;
    
    transitions.push_back(transition);
}

void StateMachine::setStateChangeCallback(std::function<void(SystemState, SystemState)> callback) {
    this->stateChangeCallback = callback;
    Serial.println("Callback de mudança de estado configurado");
}

// Métodos helper para facilitar criação de transições
void StateMachine::addTimeTransition(SystemState from, SystemState to, unsigned long timeMs, std::function<void()> transition) {
    StateCondition condition = {};
    condition.hasTimeCondition = true;
    condition.timeThreshold = timeMs;
    condition.hasBooleanCondition = false;
    condition.hasSensorCondition = false;
    
    addTransition(from, to, condition, transition);
    Serial.println("Transição por tempo: " + getStateString(from) + " -> " + getStateString(to) + " (" + String(timeMs) + "ms)");
}

void StateMachine::addWeightTransition(SystemState from, SystemState to, float weight, bool greaterThan, std::function<void()> transition) {
    StateCondition condition = {};
    condition.hasTimeCondition = false;
    condition.hasBooleanCondition = false;
    condition.hasSensorCondition = true;
    condition.sensorThreshold = weight;
    condition.greaterThan = greaterThan;
    
    addTransition(from, to, condition, transition);
    String op = greaterThan ? ">=" : "<";
    Serial.println("Transição por peso 1: " + getStateString(from) + " -> " + getStateString(to) + 
                   " (peso " + op + " " + String(weight) + " unidades)");
}

void StateMachine::addBooleanTransition(SystemState from, SystemState to, bool status, bool expectedValue, std::function<void()> transition) {
    StateCondition condition = {};
    condition.hasTimeCondition = false;
    condition.hasSensorCondition = false;
    condition.hasBooleanCondition = true;
    condition.booleanThreshold = expectedValue;

    addTransition(from, to, condition, transition);
    String expStr = expectedValue ? "true" : "false";
    Serial.println("Transição por booleano: " + getStateString(from) + " -> " + getStateString(to) + 
                   " (status esperado: " + expStr + ")");
}
