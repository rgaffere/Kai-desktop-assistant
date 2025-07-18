from sentence_transformers import SentenceTransformer, util
import torch

# Load MiniLM model
model = SentenceTransformer('all-MiniLM-L6-v2')

# Define sample phrases for each intent
intent_samples = {
    "greeting": [
        "hello kai",
        "hey there",
        "hi",
        "yo kai",
        "good morning"
    ],
    "open_app": [
        "open gmail",
        "launch brave",
        "start terminal",
        "open youtube",
        "go to drive"
    ],
    "ask_gpt": [
    "what does this mean",
    "can you explain this",
    "summarize this for me",
    "help me understand this",
    "what is going on here",
    "analyze this",
    "interpret this",
    "give me a breakdown",
    "what am I looking at",
    "describe this to me",
    "elaborate on this",
    "clarify this",
    "what is this all about",
    "make sense of this",
    "help me figure this out",
    "can you tell me what's happening",
    "explain this part",
    "give me insight",
    "I need help with this",
    "what's the idea here",
    "what do i do next"
    ],
    "do_nothing": [
    "nevermind",
    "forget it",
    "stop",
    "cancel that",
    "ignore that",
    "shut up",
    "that’s enough",
    "quiet",
    "no thanks",
    "leave me alone",
    "I'm done",
    "stop talking",
    "turn off",
    "nothing",
    "dismiss",
    "exit",
    "abort",
    "close",
    "mute",
    "don’t respond"
    ]
}

# Precompute embeddings
intent_embeddings = {
    intent: model.encode(phrases, convert_to_tensor=True)
    for intent, phrases in intent_samples.items()
}

def classify(text, threshold=0.6):
    input_embedding = model.encode(text, convert_to_tensor=True)
    
    best_intent = None
    best_score = 0.0

    for intent, embeddings in intent_embeddings.items():
        cos_sim = util.cos_sim(input_embedding, embeddings)
        max_score = torch.max(cos_sim).item()

        if max_score > best_score:
            best_score = max_score
            best_intent = intent

    if best_score < threshold:
        return "unknown", best_score

    return best_intent, best_score

import sys

if __name__ == "__main__":
    if len(sys.argv) > 1:
        user_input = sys.argv[1]
        intent, score = classify(user_input)
        print(f"{intent}:{score}")
    else:
        # Optional: test mode for CLI testing
        while True:
            user_input = input("You: ")
            intent, score = classify(user_input)
            print(f"{intent}:{score}")

