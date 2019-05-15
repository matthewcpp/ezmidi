//
//  ViewController.m
//  ios-example
//
//  Created by Matthew on 5/14/19.
//  Copyright © 2019 Matthew. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    UIPickerView *picker = [[UIPickerView alloc] init];
    picker.dataSource = self;
    picker.delegate = self;
    self.midiDeviceText.inputView = picker;
    self.midiDeviceList = [[NSMutableArray alloc] init];
    
    ezmidi_context = ezmidi_create(NULL);
    NSLog(@"Ezmidi Initialized!");
    
    int midiDeviceCount = ezmidi_get_source_count(ezmidi_context);
    
    selectedMidiDevice = 0;
    if (midiDeviceCount) {
        for (int i = 0; i < midiDeviceCount; i++) {
            [_midiDeviceList addObject:[NSString stringWithUTF8String:ezmidi_get_source_name(ezmidi_context, i)]];
        }
        
        self.midiDeviceText.text = self.midiDeviceList[0];
    }
    else {
        self.midiConnectButton.userInteractionEnabled = NO;
        self.midiConnectButton.tintColor = [UIColor grayColor];
        self.midiDeviceText.text = @"No devices found";
    }
    
    updateTimer = [NSTimer scheduledTimerWithTimeInterval: 1.0 / 60.0
                            target: self
                            selector:@selector(onTick:)
                            userInfo: nil repeats:YES];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    
    if (ezmidi_context) {
        NSLog(@"Ezmidi shutdown");
        ezmidi_destroy(ezmidi_context);
        ezmidi_context = NULL;
    }
}

-(void)onTick:(NSTimer *)timer {
    [self performSelectorOnMainThread:@selector(pumpMidiEvents) withObject:self waitUntilDone:YES];
}

-(void)pumpMidiEvents {
    Ezmidi_Event midiEvent;
    
    while (ezmidi_pump_events(ezmidi_context, &midiEvent)) {
        switch (midiEvent.type) {
            case EZMIDI_NOTE:{
                NSString* detail = midiEvent.note_event.detail == EZMIDI_NOTEEVENT_ON ? @"On" : @"Off";
                NSString* message = [NSString stringWithFormat:@"Note %@: Note %d Velocity: %d\n", detail, midiEvent.note_event.note, midiEvent.note_event.velocity];
                self.midiEventLogTextArea.text = [self.midiEventLogTextArea.text stringByAppendingString:message];
                break;
            }
        }
    }
}

- (IBAction)onConnectButton:(id)sender {
    ezmidi_connect_source(ezmidi_context, (int)selectedMidiDevice);
}

-(NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    return self.midiDeviceList.count;
}

-(NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return  1;
}

-(NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
    return self.midiDeviceList[row];
}

-(void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    selectedMidiDevice = row;
    self.midiDeviceText.text = self.midiDeviceList[row];
    [self.midiDeviceText resignFirstResponder];
}

@end
