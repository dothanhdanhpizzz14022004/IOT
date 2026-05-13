#include "tinyml.h"  
#include "global.h"
#include <Arduino.h>

namespace
{
    tflite::ErrorReporter *error_reporter = nullptr;
    const tflite::Model *model = nullptr;
    tflite::MicroInterpreter *interpreter = nullptr;
    TfLiteTensor *input = nullptr;
    TfLiteTensor *output = nullptr;

    constexpr int kTensorArenaSize = 8 * 1024;   
    uint8_t tensor_arena[kTensorArenaSize];
}

void setupTinyML()
{
    Serial.println("Initializing TensorFlow Lite Micro...");

    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(dht_anomaly_model_tflite);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
        error_reporter->Report("Model schema version mismatch! Expected %d, got %d",
                               TFLITE_SCHEMA_VERSION, model->version());
        return;
    }

    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, error_reporter);

    interpreter = &static_interpreter;

    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        error_reporter->Report("AllocateTensors() failed!");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    Serial.println("TinyML Model Loaded");
    Serial.printf("Input shape: %d features\n", input->dims->data[1]);
    Serial.printf("Output shape: %d classes\n", output->dims->data[1]);
}

String runTinyMLPrediction(float temperature, float humidity)
{
    if (interpreter == nullptr) {
        setupTinyML();
    }

    input->data.f[0] = temperature;
    input->data.f[1] = humidity;

    interpreter->Invoke();

    float anomaly_score = output->data.f[0];

    String prediction;
    float confidence = 0.0f;     

    if (humidity > 78.0f || anomaly_score > 0.68f) {
        prediction = "WET_ANOMALY";
        confidence = anomaly_score * 100;           
    }
    else if (humidity < 38.0f || anomaly_score < -0.55f) {
        prediction = "DRY_ANOMALY";
        confidence = -anomaly_score * 100;
    }
    else if (temperature > 37.5f || temperature < 15.5f) {
        prediction = "TEMP_ANOMALY";
        confidence = 75.0f;
    }
    else {
        prediction = "NORMAL";
        confidence = (1.0f - fabs(anomaly_score)) * 100;
    }

    Serial.printf(" %.1f°C |  %.1f%% | Result: %s (Inference result: %.1f%%)\n", 
                  temperature, humidity, prediction.c_str(), confidence);

    return prediction;
}

void tiny_ml_task(void *pvParameters)
{
    setupTinyML();

    Serial.println("TinyML Started");

    while (1)
    {
        if (glob_temperature > 0.0f && glob_humidity > 0.0f)
        {
            String prediction = runTinyMLPrediction(glob_temperature, glob_humidity);
            
            glob_anomaly_status = prediction;   

            Serial.printf("[TinyML] Temp=%.1f°C | Hum=%.1f%% | Prediction: %s\n", 
                          glob_temperature, glob_humidity, prediction.c_str());
        }

        vTaskDelay(pdMS_TO_TICKS(5000));   
    }
}