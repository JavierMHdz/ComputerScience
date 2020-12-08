clear
clc
close all
%update the folloing path to match your local ones
cd('/Users/javier/Downloads/hw5_data_code')
addpath('machine_learning_codes/');
load('deap_features.mat')
ctr = 0;
for part = 1:32
    for trial = 1:40
        %if the feedback record is empty the trial was missing so skip it
        if isempty(features(part,trial).feedback)
            continue
        end
        ctr = ctr+1;
        periph_feats(ctr,:) =  [features(part,trial).EMG_feats(:)' ...
            features(part,trial).GSR_feats features(part,trial).RES_feats ...
            features(part,trial).BVP_feats
            ];

        part_vect(ctr) = part;
        trial_vect(ctr) =  trial;

        %translation of reported keywords to two levels of arousal and
        %valence and likability
        %Koelstra, Sander, et al. "Deap: A database for emotion analysis; using physiological signals." Affective Computing, IEEE Transactions on 3.1 (2012): 18-31.
        arousal_feltclass(ctr) = (features(part,trial).feedback.felt_arousal>5)+1; % 1 for low and 2 for high
        valence_feltclass(ctr) = (features(part,trial).feedback.felt_valence>5)+1; % 1 for low and 2 for high
    end
    periph_feats(part_vect==part,:) = zscore(periph_feats(part_vect==part,:));

end
parameters.verbose = false;

parameters.normalize = 1;
parameters.nbClasses = 2;
parameters.lower_limit = -0.5;
parameters.upper_limit = 0.5;
parameters.grid_search = true;
parameters.featSelection = 'Fisher';
parameters.classifier = 'diagquadratic';
parameters.cross_validation = 'leave-one-out';
%ignore the nan peripheral features
periph_feats(isnan(periph_feats))=0;

for subject = 1:32
    idx = part_vect ==subject;
    if parameters.verbose
        fprintf('Classifying samples from subject %d\n',subject);
        fprintf('Peripheral signals signals\n');
    end
    %Peripheral cross validation arousal reported binary class
    [estimated_self_arousal_periph{subject}, scores_self_arousal_periph{subject}, evals_self_arousal_periph{subject}]  = ...
        cross_validation_module(periph_feats(idx,:), arousal_feltclass(idx), [], parameters);

    %Peripheral cross validation valence reported binary class
    [estimated_self_valence_periph{subject}, scores_self_valence_periph{subject}, evals_self_valence_periph{subject}]  = ...
        cross_validation_module(periph_feats(idx,:), valence_feltclass(idx), [], parameters);

    f1s_self_arousal_periph(subject) = mean(evals_self_arousal_periph{subject}.f1s);
    f1s_self_valence_periph(subject) = mean(evals_self_valence_periph{subject}.f1s);

    cr_self_arousal_periph(subject) = evals_self_arousal_periph{subject}.classification_rate;
    cr_self_valence_periph(subject) = evals_self_valence_periph{subject}.classification_rate;

end



fprintf('arousal results recognition rate = %1.2f(%1.2f) averaged F1 = %1.2f(%1.2f)\n', ...
    mean(cr_self_arousal_periph), std(cr_self_arousal_periph), mean(f1s_self_arousal_periph), std(f1s_self_arousal_periph));

fprintf('valence results recognition rate = %1.2f(%1.2f) averaged F1 = %1.2f(%1.2f)\n', ...
    mean(cr_self_valence_periph), std(cr_self_valence_periph), mean(f1s_self_valence_periph), std(f1s_self_valence_periph));


save results_DEAP evals_self_arousal_periph evals_self_valence_periph