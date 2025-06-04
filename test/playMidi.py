import mido
import serial
import time

# === CONFIGURAÇÕES ===
MIDI_FILE0 = "animals.mid"  # Exemplo de arquivo MIDI
MIDI_FILE1 = "Bohemian_Rhapsody.mid"
MIDI_FILE2 = "HarderBetterFasterStronger.mid"
MIDI_FILE3 = "Maniac.mid"  # Exemplo de arquivo MIDI
MIDI_FILE4 = "AxelF.mid"  # Exemplo de arquivo MIDI
MIDI_FILE5 = "dearly-beloved.mid"  # Exemplo de arquivo MIDI
MIDI_FILE6 = "FinalCountdown.mid"  # Exemplo de arquivo MIDI
MIDI_FILE7 = "synth.mid"  # Exemplo de arquivo MIDI
MIDI_FILE8 = "castle.mid"  # Exemplo de arquivo MIDI
MIDI_FILE9 = "gerudo.mid"  # Exemplo de arquivo MIDI
MIDI_FILE10 = "nightcall.mid"  # Exemplo de arquivo MIDI
MIDI_FILE11 = "megaman.mid"  # Exemplo de arquivo MIDI
MIDI_FILE12 = "Andor.mid"  # Exemplo de arquivo MIDI
MIDI_FILE13 = "kh.mid"  # Exemplo de arquivo MIDI

SERIAL_PORT = "/dev/ttyUSB0"        
BAUD_RATE = 500000

songs = [
    MIDI_FILE0,
    MIDI_FILE1,
    MIDI_FILE2,
    MIDI_FILE3,
    MIDI_FILE4,
    MIDI_FILE5,
    MIDI_FILE6,
    MIDI_FILE7,
    MIDI_FILE8,
    MIDI_FILE9,
    MIDI_FILE10,
    MIDI_FILE11,
    MIDI_FILE12,
    MIDI_FILE13
]
MIDI_FILE = songs[8] # Altere aqui para escolher a música
# === INICIALIZA SERIAL ===
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)
print(f"Conectado à porta {SERIAL_PORT}")

# === LÊ ARQUIVO MIDI ===
mid = mido.MidiFile(MIDI_FILE)
print(f"Lendo MIDI: {MIDI_FILE} (tempo total: {mid.length:.2f}s)")

# === TOCA MIDI ===
for msg in mid.play():  # respeita os tempos reais
    if msg.type == 'note_on' or msg.type == 'note_off':
        status = (0x90 if msg.type == 'note_on' else 0x80) | msg.channel
        note = msg.note
        velocity = msg.velocity if msg.type == 'note_on' else 0

        ser.write(bytes([status, note, velocity]))
        print(f"{msg.type.upper():8} Canal: {msg.channel} - Nota: {note}, Velocidade: {velocity}")



ser.close()
print("Fim do envio MIDI.")
