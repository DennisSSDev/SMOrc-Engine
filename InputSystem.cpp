/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/10
Description : InputSystem method definitions
----------------------------------------------*/
#include "InputSystem.h"

namespace Input {

    // Init all keyboard states to 0
    InputSystem::InputSystem()
    {
        m_KeyboardCurrent.fill(0);
        m_KeyboardPrevious.fill(0);
        m_MousePrevious.x = 0;
        m_MousePrevious.y = 0;
        m_MouseCurrent.x = 0;
        m_MouseCurrent.y = 0;

        SetDefaultKeyMap();
    }

    // Release all dynamic memory
    InputSystem::~InputSystem()
    {
        // Release keyMap and clear
        for (auto pair : m_keyMap)
            delete pair.second;
        m_keyMap.clear();

        // No need to delete dynamic memory from activeKeyMap, it's already deleted in keymap*
        m_activeKeyMap.clear();
    }

    void InputSystem::Frame(float dt, Camera* camera)
    {
        float speed = camera->GetMovementSpeed() * dt;

        // Act on user input:
        // - Iterate through all active keys
        // - Check for commands corresponding to activated chords
        // - Do something based on those commands
        for (auto pair : m_activeKeyMap)
        {
            switch (pair.first)
            {
            case GameCommands::Quit:
                PostQuitMessage(0);
                break;
            case GameCommands::MoveForward:
                camera->GetTransform()->MoveRelative(0.0f, 0.0f, speed);
                break;
            case GameCommands::MoveBackward:
                camera->GetTransform()->MoveRelative(0.0f, 0.0f, -speed);
                break;
            case GameCommands::MoveLeft:
                camera->GetTransform()->MoveRelative(-speed, 0.0f, 0.0f);
                break;
            case GameCommands::MoveRight:
                camera->GetTransform()->MoveRelative(speed, 0.0f, 0.0f);
                break;
            case GameCommands::CameraRotation:
            {
                std::pair<float, float> delta = this->GetMouseDelta();

                float mouseSensitivity = camera->GetSensitivity();

                delta.first *= mouseSensitivity * dt;
                delta.second *= mouseSensitivity * dt;

                
                camera->GetTransform()->Rotate(delta.second, delta.first, 0.0f);
                break;
            }
            }
        }
        
        GetInput();
    }

    // Stores previous/current keymappings (from windows)
    // Updates active keymap by comparing all chords 
    // Updates mouse mapping
    void InputSystem::GetInput()
    {
        // Get the keyboard state from windows
        GetKeyboardState();

        // Update active/non-active keymaps
        UpdateKeymaps();

        // Update mouse state
        m_MousePrevious = m_MouseCurrent;
    }

    void InputSystem::OnMouseMove(short newX, short newY)
    {
        m_MouseCurrent = { newX, newY };
    }

    std::pair<float,float> InputSystem::GetMouseDelta() const
    {
        std::pair<float, float> pt;
        
        pt.first  = static_cast<float>(m_MouseCurrent.x - m_MousePrevious.x);
        pt.second = static_cast<float>(m_MouseCurrent.y - m_MousePrevious.y);
        return pt;
    }

    // Clears active key map, then fills it with all values from m_keyMap with a 'fulfilled' chord
    void InputSystem::UpdateKeymaps()
    {
        // Reset active key map
        m_activeKeyMap.clear();

        // Map which keys are active into the active key map
        for (auto key : m_keyMap)
        {
            bool activeKey = true;

            // Test Chord
            for (Binding binding : key.second->GetChord())
            {
                if (GetKeyboardKeyState(binding.m_KeyCode) != binding.m_KeyState)
                {
                    activeKey = false;
                    break;
                }
            }

            // Passed Chord Check : move key to active key map
            if (activeKey)
                m_activeKeyMap.insert(std::pair<GameCommands, Chord*>(key.first, key.second));
        }
    }

    // Stores the current state of the keyboard as 'previous'
    // then reads in new 'current values from windows
    void InputSystem::GetKeyboardState()
    {
        m_KeyboardPrevious = m_KeyboardCurrent;

        for (int i = 0; i < 256; i++)
            m_KeyboardCurrent[i] = isPressed(i);
    }

    // Use logic to deduce Keystate from current and previous keyboard states
    const KeyState InputSystem::GetKeyboardKeyState(const unsigned int a_keyCode) const
    {
        if (m_KeyboardPrevious[a_keyCode] == 1)
            if (m_KeyboardCurrent[a_keyCode] == 1)
                return KeyState::StillPressed;  // true, true
            else
                return KeyState::JustReleased;  // true, false
        else
            if (m_KeyboardCurrent[a_keyCode] == 1)
                return KeyState::JustPressed;   // false, true
            else
                return KeyState::StillReleased; // false, false
    }
    
    // Set the default key bindings with human readable names
    void InputSystem::SetDefaultKeyMap()
    {
        m_keyMap.clear();
        m_keyMap[GameCommands::Quit]         = new Chord(L"Quit", VK_ESCAPE, KeyState::JustReleased);
        m_keyMap[GameCommands::MoveForward]  = new Chord(L"Move Forward", 'W', KeyState::StillPressed);
        m_keyMap[GameCommands::MoveBackward] = new Chord(L"Move Backward", 'S', KeyState::StillPressed);
        m_keyMap[GameCommands::MoveLeft]     = new Chord(L"Move Left", 'A', KeyState::StillPressed);
        m_keyMap[GameCommands::MoveRight]    = new Chord(L"Move Right", 'D', KeyState::StillPressed);

        m_keyMap[GameCommands::CameraRotation] = new Chord(L"Camera Rotation", VK_RBUTTON, KeyState::StillPressed);
    }
}